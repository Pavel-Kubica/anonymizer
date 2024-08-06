SET allow_experimental_live_view=1;

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
    url String
)
ENGINE=MergeTree()
ORDER BY (resource_id, remote_addr, cache_status, response_status);

CREATE LIVE VIEW log_counts_by_ri
AS SELECT
       resource_id,
       count() AS total
    FROM http_log
    GROUP BY resource_id
    ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_rs
AS SELECT
       response_status,
       count() AS total
    FROM http_log
    GROUP BY response_status
    ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_cs
AS SELECT
       cache_status,
       count() AS total
    FROM http_log
    GROUP BY cache_status
    ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_ra
AS SELECT
       remote_addr,
       count() AS total
    FROM http_log
    GROUP BY remote_addr
    ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_ri_rs
AS SELECT
       resource_id,
       response_status,
       count() AS total
   FROM http_log
   GROUP BY resource_id, response_status
   ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_ri_cs
AS SELECT
       resource_id,
       cache_status,
       count() AS total
   FROM http_log
   GROUP BY resource_id, cache_status
   ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_ri_ra
AS SELECT
       resource_id,
       remote_addr,
       count() AS total
   FROM http_log
   GROUP BY resource_id, remote_addr
   ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_rs_cs
AS SELECT
       response_status,
       cache_status,
       count() AS total
   FROM http_log
   GROUP BY response_status, cache_status
   ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_rs_ra
AS SELECT
       response_status,
       remote_addr,
       count() AS total
   FROM http_log
   GROUP BY response_status, remote_addr
   ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_cs_ra
AS SELECT
       cache_status,
       remote_addr,
       count() AS total
   FROM http_log
   GROUP BY cache_status, remote_addr
   ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_ri_rs_cs
AS SELECT
       resource_id,
       response_status,
       cache_status,
       count() AS total
   FROM http_log
   GROUP BY resource_id, response_status, cache_status
   ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_ri_rs_ra
AS SELECT
       resource_id,
       response_status,
       remote_addr,
       count() AS total
   FROM http_log
   GROUP BY resource_id, response_status, remote_addr
   ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_ri_cs_ra
AS SELECT
       resource_id,
       cache_status,
       remote_addr,
       count() AS total
   FROM http_log
   GROUP BY resource_id, cache_status, remote_addr
   ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_rs_cs_ra
AS SELECT
       response_status,
       cache_status,
       remote_addr,
       count() AS total
   FROM http_log
   GROUP BY response_status, cache_status, remote_addr
   ORDER BY total DESC;

CREATE LIVE VIEW log_counts_by_all
AS SELECT
       resource_id,
       response_status,
       cache_status,
       remote_addr,
       count() AS total
   FROM http_log
   GROUP BY resource_id, response_status, cache_status, remote_addr
   ORDER BY total DESC;