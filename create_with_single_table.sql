
CREATE TABLE http_log
(
    timestamp DateTime,
    resource_id UInt64,
    bytes_sent UInt64,
    request_time_milli UInt64,
    response_status UInt16,
    cache_status LowCardinality(String),
    method LowCardinality(String),
    remote_addr String,
    url String,

)
ENGINE=MergeTree()
ORDER BY (resource_id, remote_addr, cache_status, response_status);

CREATE TABLE log_counts
(
    resource_id UInt64,
    response_status UInt16,
    cache_status LowCardinality(String),
    remote_addr String,
    total AggregateFunction(count)
)
ENGINE=AggregatingMergeTree()
ORDER BY (resource_id, response_status, cache_status, remote_addr);
CREATE MATERIALIZED VIEW log_count_mv TO log_counts
AS
SELECT
    resource_id,
    response_status,
    cache_status,
    remote_addr,
    countState() AS total
FROM http_log
GROUP BY
    resource_id,
    response_status,
    cache_status,
    remote_addr