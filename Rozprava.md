# Rozprava

## Výběr úlohy

Zprvu jsem chtěl dělat úlohu s envoyem, ale už jenom z toho jak dlouho trvala kompilace jsem od tohoto nápadu rychle upustil na jedinou druhou úlohu, která
mi přišla schůdná.

## Výběr knihoven

- Librdkafka se jevila jako nejjednodušší možnost pro čtení zpráv z Kafky.
- Libcpr jsem již znal a měl jsem s ní dobré zkušenosti z předchozího projektu pro posílání HTTP requestů.

## Rozhodnutí, problémy, řešení

### Consumování zpráv z Kafky

Hned ze začátku jsem měl jasno, že consumovat bude separátní vlákno, a zprávy bude dávat do vnější fronty. Při nastavování RdKafka::Consumer
jsem měl malé problémy jaký partition zvolit a jak nakonfigurovat IP. URL a Partition jsem vyřešil metodou pokus-omyl, dokud jsem nenašel kombinaci,
která funguje. Nutnou konfiguraci nebyl problém najít na internetu.

### Transformace zpráv z Kafky

Consumované zprávy jsem od začátku chtěl ukládat do souboru, abych o logy nepřišel v případě pádu programu, a aby mi nedošla paměť
kdyby databáze delší dobu nepřijímala požadavky a zpráv by chodilo hodně. Původně jsem chtěl zprávy psát do souboru přímo z RdKafka::Message, ale
takto zapsané zprávy již poté nebylo schopné capnp dekódovat. Proto zprávy již z fronty dekóduji, již tam anonymizuji IP, a až poté ukládám do souboru,
odkud si je jednou za minutu znovu přečte další část programu. Narazil jsem na problém, že payload zpráv z Kafky původně nemohlo capnp dekódovat, ale
s radou, o kterou jsem si napsal, jsem tento problém (payload zpráv nebyl word-aligned) vyřešil.

### Zápis do databáze

Proxy pro databázi by sice měla mít timeout 60 sekund, ale abych dosáhl úspěšného requestu pokaždé, posílací část programu se spouští každých 63 sekund.
Ze souborů se postupně čtou dříve zapsané logy, během tohoto čtení je již pro zapisování vytvořen a používán nový soubor. Z capnp formátu se logy 
interpretují na formát pro SQL insert příkaz, a až jsou všechny logy ze všech souborů přečtené, je příkaz proveden. Pokud uspěje, přečtené soubory
jsou smazány, pokud selže, zůstávají na disku, a po dalším intervalu jsou znovu přečteny a jejich logy odeslány. Bylo by možné pro posílání ukládat
logy rovnou v paměti zároveň s ukládáním v souboru, ale tím by opět hrozilo vyčerpání paměti pokud by logů bylo hodně a připojení k databázi často selhávalo.

### Optimalizace databáze

Na tuto část úlohy jsem po nějakém přemýšlení šel s myšlenkou že si předpočítám počty pro každou čtveřici žádaných parametrů, a při každém insertu
budu tyto počty updatovat.
Idea: 
```
    Tabulka: [resource_id, response_status, cache_status, remote_addr, count]
    Při každém insertu do http_log se buď 
        Chování 1 - count pro odpovídající řádky zvedne, nebo
        Chování 2 - se refreshne celá tahle tabulka.
```
Později jsem si také uvědomil, že bude lepší (a zadání vyžaduje) předpočítat všechny počty a mít na ně 15 tabulek (2^4 - 1). Protože tyto tabulky ukládají jenom
podmnožinu dat, udrží se výsledná velikost tabulek na disku stále z mého pohledu přípustná.

Hned první způsob, jaký jsem pro toto zkoušel, byl skrze projekce, avšak z nějakého důvodu při prvním pokusu jsem s tímto narazil na problémy, a tak jsem 
dlouhou dobu (~25h čistého času) hledal alternativy přes Materialized Views, Live Views, tabulky se sloupci typu AggregateFunction atd. a přemýšlel jak jinak 
by se to dalo udělat. S MatViews (a zprvu i s projekcemi) jsem měl problém, že se výsledky count() funkce nesčítaly ke stejnému řádku, ale místo toho se vytvořil
nový. Sice se mi povedlo toto vyřešit přes sloupce typu AggregateFunction, ale podle těchto zase nešlo řadit, a tak jako tak tímto přístupem musím mít výsledky
ve větším množství tabulek. Spojení všech výsledků do jedné tabulky s nullable sloupci jsem pro jistotu také zkusil, ale ve výsledku to pouze bylo několikrát
pomalejší, než prostý SELECT z raw dat. Nakonec jsem znovu zkusil projekce, a už se mi s nimi snad povedlo zadání splnit. Do projekcí se sice také vkládají po 
insertech do hlavní tabulky nové řádky místo zvedání totalu u původních, ale select, kterým nakonec totaly získáváme, tyto řádky spojí, a vnitřní implementace
dle pozorování sama tyto řádky spojuje když má projekce více než 10 parts.

Další optimalizace s pomocí řazení řádků v tabulkách podle totalů se mi nepodařilo dosáhnout, nejsem si jist, jestli je něco takového vůbec v ClickHouse možné.
Bylo by potřeba nějak při každém INSERTu celou tu tabulku s totaly vyprázdnit a znovu vyplnit, ale MatViews a projekce se koukají pouze na nově insertnuté řádky.
(Alespoň dodaný) server také nepodporuje multiquery, takže nelze v insertujícím requestu zároveň vytvářet tabulky (nehledě na to, že v produkčním prostředí by
na toto nemusela být dostatečná práva).

## Strávený čas

**C++ část**
- ~15h porozumění technologiím (Kafka, Capnp, CH proxy)
- ~15h implementace funkcionality
- ~5h debug + testování

**ClickHouse část**
- ~25h snaha porozumět různým konstrukcím, experimenty s nimi, zjišťování, jaké využít.

~5h 

#### Celkem

65h