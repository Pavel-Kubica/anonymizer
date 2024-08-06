# Report

## Failure points

### No messages received from kafka

Warning issued, program continues running

### DB write fails (network/server error)

Logs will continue to be stored in temporary files until they can be successfully written.
Only after that will they be deleted.

## Limitations

### Lost data

If the application can pick up the record from Kafka, the only way that it can get lost at that point is if the process is killed
and the record was not written to the temporary file/database yet. Opportunities for data loss are minimal, unsure if they could even be
further minimized.

### Duplicate data

TODO figure out if messages from kafka get consumed again

### Latency

TODO benchmark how long between consume and write end

## Table architecture efficiency

### Key limitations and their consequences

Unfortunately for our use case ("... on-demand rendering of traffic charts in a front-end such as Grafana"), we are very limited in ways for
a ClickHouse table to store the results of aggregate functions, updating them on the fly with inserts, and at the same time for that table
to be ordered by those results of aggregate functions. In other words, to pre-compute totals and pre-order the rows by those totals. 
Naturally, to achieve such behavior, the desired table would have to be fully rebuilt upon every `INSERT` to `http_log`. `PROJECTION`s and `MATERIALIZED VIEW`s
only consider the newly inserted elements, therefore we cannot use `count()` directly, instead going through a column of type `AggregateFunction`, which
however we cannot order by. A view or projection targeting this column with countMerge(total) also becomes desynchronized after `INSERT`s to `http_log`, further
complicating the goal of having everything in one table.

Seemingly the only storage type which actually fully rebuilds itself upon `INSERT` to its target table is `LIVE VIEW`, an experimental/deprecated feature.
Besides the obvious issues with using a feature marked in those ways, it is also impossible to consolidate the results into a single table for simple selects
while preserving the gained speed.

If however we do not use `LIVE VIEW`s, the above limitations mean that to select for example the top 100 most frequently requested resources, we will
need to go through the totals for all resources. The row count of a table holding these totals for a subset of the four parameters X will be
SELECT count(DISTINCT X) FROM http_log, therefore any accurate query will always have to scan at least this many rows before returning a result.

Because `PROJECTION`s don't allow us to properly update and group the results of aggregate functions, there is also the problem of how to consolidate
results into one table. A table with nullable keys would accumulate an extreme row count of up to 12 times that of `http_log`, therefore it is not an option.
In the end there simply seems to be no good way of having everything in one table. To still have the option and allow for simple selects that way, a simple
architecture with pre-computed totals for combinations of all 4 parameters (and nothing else) is provided.

The slower, single table approach that doesn't use `LIVE VIEW`s will be referred to as table architecture #1 and can be chosen by initializing
the database with [create_with_single_table.sql](create_with_single_table.sql).
The fast but messy approach will be referred to as table architecture #2, and implemented
in [create_with_multiple_tables.sql](create_with_multiple_tables.sql).

Because we have to pre-define the charts to show in Grafana anyway, having the results in multiple tables shouldn't be a significant issue.

*SIDENOTE*

Depending on our expected dataset, the totals for any combination of the four parameters could be high, in which case it makes perfect sense to pre-compute them,
or they could be in the majority of cases close to 1, in which case the table holding these totals will be very large, and at that point it could make sense
to sacrifice some query speed to save on a significant amount of disk space. The decision in our architecture is to pre-compute all results though.

### Speed



### Disk space

Provide an estimate of disk space required given
1) average incoming message rate
2) retention of the aggregated data