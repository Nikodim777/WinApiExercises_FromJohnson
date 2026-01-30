# Comparison of copy functions perfomance
Two measurements were taken for each test, average used as result.
All results are in microseconds.
The results determined on medium hardware and old hdd.
Seconds are separated by a space.
In my own functions the buffer size is 260 bytes.

|Function|Description|5kb file|9mb file|350mb file|
|:------:|:---------:|-------:|-------:|---------:|
|cpC|The C realization|5400|45950|6 157000|
|cpCpp|The C++ realization|28600|19 220700|749 424700|
|cpCWin|The C real. with winapi|16000|280000|11 500000|
|cpCWinNB|The C real. with winapi without system buffer|22250|240000|97 500000|
|cpCWin with FLAF_SEQUENTIAL|The C real. with winapi|8250|277000|18 170000|
|cpCWinNB with FLAF_SEQUENTIAL|The C real. with winapi without system buffer|9400|255000|96 500000|
|cpWinAux|The winapi CopyFile|7000|10000|150000|
