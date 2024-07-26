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

TODO benchmark

Characterize the aggregated select query time to show the table architecture is fit for purpose.
Provide an estimate of disk space required given
1) average incoming message rate
2) retention of the aggregated data