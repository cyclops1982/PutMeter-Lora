# PutMeter - Lora

Lora PutMeter. Measures the distance and reports it to lora.

## Device

The RAK WisBlock modules used:

- RAK19009 (WisBlock Mini Base Board)
- RAK4631 (WisBlock LPWAN Module Nordic nRF52840)

## Goals

Measure the distance, and calculate how 'full' a tank/put is.

## Dev Environment
We're using VSCode and have added the .vscode folder as well to make it easy.
Also note that the git repo has submodules, so you'll need to do a `git submodule init && git submodule update` after the clone.


## Error notes

The thing will blink a number of times when it fails. The blinks indicate what's wrong:
```
2 = Initialization of config failed

```

## BlinkStatus

In some cases, we blink a status code. These are the various status blinks:

```
2 = Lora Join failed
3 = Lora send failed
```
