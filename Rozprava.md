# Rozprava

## Výběr úlohy

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
odkud si je jednou za minutu znovu přečte další část programu. Narazil jsem na problém, že payload zpráv z Kafky původně nemohlo capnp dekódovat, ale s radou
, o kterou jsem si napsal, jsem tento problém (payload zpráv nebyl word-aligned) vyřešil.

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
    4 tyhle tabulky, každá seřazená podle jiného ze 4 parametrů, ideálně skrz projection seskupeno aby se dalo v grafaně selectovat z jedné tabulky.
    Při hledání podle různých kombinací už určitě bude ořízlá tabulka dostatečně malá na full table scan
```
Nad tímto chováním jsem strávil značné množství času s PROJECTION a MATERIALIZED VIEW, než jsem konečně přišel na tabulku se sloupcem typu AggregateFunction.


60h

