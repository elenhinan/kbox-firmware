/*
  The MIT License

  Copyright (c) 2016 Thomas Sarlandie thomas@sarlandie.net

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
#include <SdFat.h>
#include "board.h"
#include "SdcardTestPage.h"

SdcardTestPage::SdcardTestPage() {
  status = -1;

  if (!sd.begin(sdcard_cs)) {
    if (sd.card()->errorCode()) {
      DEBUG("Something went wrong ... SD card errorCode: %i errorData: %i", sd.card()->errorCode(), sd.card()->errorData());
      return;
    }
    else {
      DEBUG("SdCard successfully initialized.");
    }

    if (sd.vol()->fatType() == 0) {
      DEBUG("Can't find a valid FAT16/FAT32 partition. Try reformatting the card.");
      return;
    }

    if (!sd.vwd()->isOpen()) {
      DEBUG("Can't open root directory. Try reformatting the card.");
      return;
    }

    DEBUG("Unknown error while initializing card.");
    return;
  }

  uint32_t size = sd.card()->cardSize();
  if (size == 0) {
    DEBUG("Can't figure out card size :(");
    return;
  }
  uint32_t sizeMB = 0.000512 * size + 0.5;
  DEBUG("Card size: %luMB. Volume is FAT%i Cluster size: %i bytes", sizeMB, sd.vol()->fatType(), 512 * sd.vol()->blocksPerCluster());

 if ((sizeMB > 1100 && sd.vol()->blocksPerCluster() < 64)
      || (sizeMB < 2200 && sd.vol()->fatType() == 32)) {
    DEBUG("This card should be reformatted for best performance.");
    DEBUG("Use a cluster size of 32 KB for cards larger than 1 GB.");
    DEBUG("Only cards larger than 2 GB should be formatted FAT32.");
    return;
  }

  DEBUG("Listing files...");
  sd.ls(LS_R | LS_DATE | LS_SIZE);
}

void SdcardTestPage::readCard() {

  needsPainting = true;
}

void SdcardTestPage::willAppear() {
  readCard();
}

bool SdcardTestPage::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypePressed) {
    readCard();
    return true;
  }
  return false;
}

void SdcardTestPage::paint(GC &gc) {
  if (needsPainting) {

    needsPainting = false;
  }
}
