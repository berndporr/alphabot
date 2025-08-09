# Robot remote control using fastDDS by a logitech steering wheel

## Prereqisites

Fast CDR
```
apt install libfastcdr-dev
```

Fast RTPS
```
apt install libfastrtps-dev
```

Fast DDS tools
```
apt install fastddsgen fastdds-tools
```

## How to compile

```
cmake .
make
```


## How to run

Transmitter: Open a terminal and type
```
./remote
```

Test receiver: Open another terminal and type

```
./test_robo_msg_sub
```
