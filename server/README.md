# Overview

This is a very simple server that will receive raw audio and write it to a file.

It will write two different files depending on what it receives:

- `adc.raw` - samples from the analog to digital converter.
- `i2s.raw` - samples from the I2S microphone.

Samples are appended to any existing file so if you want to start again from scratch you will need to delete the raw files.

To import the files use the "raw" import option on Audacity - this will let you specify the size of the samples (signed 16 bit), number of channels (1 or 2 depending on if you are capturing one or two channels), and sample rate (16KHz is the default in the code).

# Prerequisites

You will need to have [node](https://nodejs.org/en/download/) and [yarn](https://classic.yarnpkg.com/en/docs/install/#mac-stable). You may already have these on your system.

Check with:

```
node --version
yarn --version
```

# Setup

To install the dependencies run:

```
cd server
yarn
```

# Running the server

The following commands will run the server:

```
cd server
yarn start
```
