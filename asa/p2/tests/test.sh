#!/usr/bin/env bash

d="$(dirname "$0")"
echo > $d/myout
for t in {0..1000}; do
    echo -ne "$t/1000"\\r
    $d/../p2 < $d/$t.in >> $d/myout
done
diff $d/out $d/myout
