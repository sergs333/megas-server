/*
  Modul SD card for Due
  part of Arduino Mega Server project
*/

void sdCardInit() {
  Serialprint("Modul SD...\n");
  
  Serialprint(" SD started... ");
  if (SD.begin(SD_CHIP_SELECT)) {
    Serialprint("OK\n");
  } else {
      Serialprint("failed\n");
      return;
    }
    
  if (SD.exists("index.htm")) {
    Serialprint(" index.htm... found\n");
  } else {
      Serialprint(" Can't find index.htm\n");
      return;      
    }
    
  modulSdCard = MODUL_ENABLE;
  Serialprint("started\n");
}

#ifdef SD_INFO_FEATURE
  void sdInfoInit() {
    timeStamp();
    Serialprint("Modul SD Info... ");
    if (card.init(SPI_FULL_SPEED, SD_CHIP_SELECT)) {
      Serialprint("started\n");
      return;
    } else {
        Serialprint("failed\n");
      }
  }
  
  byte sdType() {
    switch(card.type()) {
      case SD_CARD_TYPE_SD1:  return 1; break;
      case SD_CARD_TYPE_SD2:  return 2; break;
      case SD_CARD_TYPE_SDHC: return 3; break;
      default: return 0;
    }
  }

  byte sdFileSystem() {
    if (volume.init(card)) {return volume.fatType();}
                      else {return 0;}
  }

  int sdVolumeSize() {
    if (volume.init(card)) {
      uint32_t volumesize;
      volumesize = volume.blocksPerCluster();
      volumesize *= volume.clusterCount();
      volumesize *= 512;         // size in bytes
      volumesize /= 1024;        // size in Kbytes
      return volumesize /= 1024; // size in Mbytes
    } else {
        return 0;
      }
  }

  void cardInfo() {
    Serial.print("\nCard type: ");
    switch(card.type()) {
      case SD_CARD_TYPE_SD1: Serialprint("SD1\n"); break;
      case SD_CARD_TYPE_SD2: Serialprint("SD2\n"); break;
      case SD_CARD_TYPE_SDHC:Serialprint("SDHC\n");break;
      default: Serialprint("Unknown\n");
    }

    // try to open the partition FAT16 or FAT32
    if (!volume.init(card)) {
      Serialprint("Not find partition\n");
      return;
    }

    // print the type and size of the first FAT-type volume
    uint32_t volumesize;
    Serial.print("\nVolume type is FAT");
    Serial.println(volume.fatType(), DEC);
    Serial.println();
 
    volumesize = volume.blocksPerCluster(); // clusters are collections of blocks
    volumesize *= volume.clusterCount(); // we'll have a lot of clusters
    volumesize *= 512; // SD card blocks are always 512 bytes
    Serialprint("Volume size (bytes): ");
    Serial.println(volumesize);
    Serialprint("Volume size (Kbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);
    Serialprint("Volume size (Mbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);

    Serialprint("\nFound files:\n");
    root.openRoot(volume);
 
    // list files in the card
    root.ls(LS_R | LS_DATE | LS_SIZE);
  }
#endif // SD_INFO_FEATURE

#ifdef SD_FILES_FEATURE
  void sdFilesInit() {
    sdRoot = SD.open("/");
    sdDir = SD.open("/");
    started("SD Files");
  }

  void printSdContent(File dir, int numTabs) {
     while(true) {
       File entry =  dir.openNextFile();
       if (! entry) {
         dir.rewindDirectory();
         break;
       } // no more files
       for (uint8_t i = 0; i < numTabs; i++) {
         Serial.print('\t');
       }
       Serial.print(entry.name());
       if (entry.isDirectory()) {
         Serialprint("/\n");
         printSdContent(entry, numTabs + 1);
       } else {
           Serial.print("\t");
           Serial.println(entry.size(), DEC);
         }
       entry.close();
     }
  }  // printSdContent( )

  void printDirectory(File dir) {
     while(true) {
       File entry = dir.openNextFile();
       if (!entry) {break;} // no more files
       if (!entry.isDirectory()) {
         Serial.println(entry.name());
       } 
       entry.close();
     }
  }

  String strDirectory(File dir) {
    String str = "";
    dir.rewindDirectory();
    while(true) {
      entry = dir.openNextFile();
      if (! entry) { // no more files
        dir.rewindDirectory();
        return str;
        break;
      }
      if (! entry.isDirectory()) {
        str += entry.name();
        str += " ";
      } 
      entry.close();
    }
  }
#endif // SD_FILES_FEATURE
