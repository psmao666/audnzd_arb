# audnzd_arb
Arbitrage on AUDNZD

First, compile the backtester

```
g++ -std=c++2a -o test backtester.cpp
```

Then, run it with 
```
./test
```
and you will get a report.txt file, now compile crawler.cpp with
```
g++ -std=c++2a -o crawler crawler.cpp
```
and run
```
./crawler
```
to obtain portfolio.txt, and lastly, run
```
python3 plot.py
```
to visualise the result.