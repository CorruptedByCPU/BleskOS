//BleskOS

/*
* MIT License
* Copyright (c) 2023-2024 Vendelín Slezák
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

void initalize_storage_controllers(void) {
 //now we support only one hard disk and one optical drive
 hard_disk_info.controller_type = NO_CONTROLLER;
 optical_drive_info.controller_type = NO_CONTROLLER;

 //allocate memory for IDENTIFY command data
 device_info = (struct ata_identify_command_device_info *) (malloc(512));

 //detect all storage devices
 log("\n");
 for(dword_t i=0; i<number_of_storage_controllers; i++) {
  if(storage_controllers[i].controller_type==IDE_CONTROLLER) {
   initalize_ide_controller(i);
  }
  else if(storage_controllers[i].controller_type==AHCI_CONTROLLER) {
   initalize_ahci_controller(i);
  }
 }

 //free memory
 free((dword_t)device_info);

 //log
 log("\n");
 if(hard_disk_info.controller_type!=NO_CONTROLLER) {
  log("\nHard disk: ");
  if(hard_disk_info.controller_type==IDE_CONTROLLER) {
   log("IDE ");
   log_hex_specific_size_with_space(hard_disk_info.base_1, 4);
   log_hex_specific_size_with_space(hard_disk_info.device_port, 2);
  }
  else if(hard_disk_info.controller_type==AHCI_CONTROLLER) {
   log("AHCI ");
   log_hex_with_space(hard_disk_info.base_1);
   log("port ");
   log_var_with_space(hard_disk_info.device_port);
  }
  log("Number of sectors: ");
  log_var(hard_disk_info.number_of_sectors);
 }

 if(optical_drive_info.controller_type!=NO_CONTROLLER) {
  log("\nOptical drive: ");
  if(optical_drive_info.controller_type==IDE_CONTROLLER) {
   log("IDE ");
   log_hex_specific_size_with_space(optical_drive_info.base_1, 4);
   log_hex_specific_size_with_space(optical_drive_info.device_port, 2);
  }
  else if(optical_drive_info.controller_type==AHCI_CONTROLLER) {
   log("AHCI ");
   log_hex_with_space(optical_drive_info.base_1);
   log("port ");
   log_var_with_space(optical_drive_info.device_port);
  }
 }
}

byte_t if_storage_medium_exist(byte_t type_of_medium, byte_t medium_number) {
 if(type_of_medium==MEDIUM_HDD && medium_number==DEFAULT_MEDIUM) {
  if(hard_disk_info.controller_type!=NO_CONTROLLER) {
   return STATUS_TRUE;
  }
 }
 
 if(type_of_medium==MEDIUM_CDROM && medium_number==DEFAULT_MEDIUM) {
  if(optical_drive_info.controller_type!=NO_CONTROLLER) {
   return STATUS_TRUE;
  }
 }
 
 if(type_of_medium==MEDIUM_USB_MSD) {
  if(usb_mass_storage_devices[medium_number].type==USB_MSD_INITALIZED) {
   return STATUS_TRUE;
  }
 }
 
 return STATUS_FALSE;
}

void select_storage_medium(byte_t type_of_medium, byte_t medium_number) {
 storage_medium = 0;
 storage_medium_number = 0;

 if(type_of_medium==MEDIUM_HDD && medium_number==DEFAULT_MEDIUM) {
  storage_medium = MEDIUM_HDD;
  storage_medium_number = DEFAULT_MEDIUM;

  //select HDD
  if(hard_disk_info.controller_type==AHCI_CONTROLLER) { //hard disk is connected to AHCI port
   return;
  }
  else if(hard_disk_info.controller_type==IDE_CONTROLLER) { //hard disk is connected to IDE port
   ide_select_drive(hard_disk_info.base_1, hard_disk_info.device_port);
   return;
  }
 }
 
 if(type_of_medium==MEDIUM_CDROM && medium_number==DEFAULT_MEDIUM) {
  storage_medium = MEDIUM_CDROM;
  storage_medium_number = DEFAULT_MEDIUM;
  
  //select CDROM
  if(optical_drive_info.controller_type==AHCI_CONTROLLER) { //CDROM is connected to AHCI port
   return;
  }
  if(optical_drive_info.controller_type==IDE_CONTROLLER) { //CDROM is connected to IDE port
   ide_select_drive(optical_drive_info.base_1, optical_drive_info.device_port);
   return;
  }
 }
 
 if(type_of_medium==MEDIUM_USB_MSD) {
  storage_medium = MEDIUM_USB_MSD;
  storage_medium_number = medium_number;
  return;
 }
}

byte_t read_storage_medium(dword_t sector, byte_t num_of_sectors, dword_t memory) {
 if(storage_medium==MEDIUM_HDD) {
  //read HDD
  if(hard_disk_info.controller_type==AHCI_CONTROLLER) { //hard disk is connected to AHCI port
   return sata_read(hard_disk_info.base_1, hard_disk_info.base_2, sector, num_of_sectors, memory);
  }
  if(hard_disk_info.controller_type==IDE_CONTROLLER) { //hard disk is connected to IDE port
   return pata_read(hard_disk_info.base_1, sector, num_of_sectors, memory);
  }
 }
 
 if(storage_medium==MEDIUM_CDROM) {
  //read CDROM
  if(optical_drive_info.controller_type==AHCI_CONTROLLER) { //CDROM is connected to AHCI port
   return satapi_read(optical_drive_info.base_1, optical_drive_info.base_2, sector, num_of_sectors, memory);
  }
  if(optical_drive_info.controller_type==IDE_CONTROLLER) { //CDROM is connected to IDE port
   return patapi_read(optical_drive_info.base_1, optical_drive_info.base_2, sector, num_of_sectors, memory);
  }
 }
 
 if(storage_medium==MEDIUM_USB_MSD) {
  if(usb_mass_storage_devices[storage_medium_number].type!=USB_MSD_INITALIZED) {
   return STATUS_ERROR; //there is no USB MSD connected
  }
  
  if(usb_mass_storage_devices[storage_medium_number].size_of_sector==512) {
   return usb_msd_read(storage_medium_number, sector, num_of_sectors, memory);
  }
  else {
   return STATUS_ERROR; //TODO: we do not support another sector size
  }
 }
 
 return STATUS_ERROR;
}

byte_t read_audio_cd(dword_t sector, dword_t num_of_sectors, dword_t memory) {
 if(storage_medium==MEDIUM_CDROM) {
  if(optical_drive_info.controller_type==AHCI_CONTROLLER) { //CDROM is connected to AHCI port
   return satapi_read_audio_cd_sector(optical_drive_info.base_1, optical_drive_info.base_2, sector, num_of_sectors, memory);
  }
  if(optical_drive_info.controller_type==IDE_CONTROLLER) { //CDROM is connected to IDE port
   return patapi_read_audio_cd_sector(optical_drive_info.base_1, optical_drive_info.base_2, sector, num_of_sectors, memory);
  }
 }

 return STATUS_ERROR;
}

byte_t write_storage_medium(dword_t sector, byte_t num_of_sectors, dword_t memory) {
 if(storage_medium==MEDIUM_HDD) {
  //write HDD
  if(hard_disk_info.controller_type==AHCI_CONTROLLER) { //hard disk is connected to AHCI port
   return sata_write(hard_disk_info.base_1, hard_disk_info.base_2, sector, num_of_sectors, memory);
  }
  if(hard_disk_info.controller_type==IDE_CONTROLLER) { //hard disk is connected to IDE port
   return pata_write(hard_disk_info.base_1, sector, num_of_sectors, memory);
  }
 }
 
 if(storage_medium==MEDIUM_CDROM) {
  return STATUS_ERROR; //we do not support CDROM write
 }
 
 if(storage_medium==MEDIUM_USB_MSD) {
  if(usb_mass_storage_devices[storage_medium_number].type!=USB_MSD_INITALIZED) {
   return STATUS_ERROR; //there is no USB MSD connected
  }
  
  if(usb_mass_storage_devices[storage_medium_number].size_of_sector==512) {
   return usb_msd_write(storage_medium_number, sector, num_of_sectors, memory);
  }
  else {
   return STATUS_ERROR; //TODO: we do not support another sector size
  }
 }
 
 return STATUS_ERROR;
}

byte_t detect_optical_disk(void) {
 byte_t status;
 optical_disk_size = 0;
 
 //select medium
 if(if_storage_medium_exist(MEDIUM_CDROM, DEFAULT_MEDIUM)==STATUS_FALSE) {
  return STATUS_FALSE;
 }
 
 save_value_storage_medium = storage_medium;
 save_value_storage_medium_number = storage_medium_number;
 
 if(storage_medium!=MEDIUM_CDROM) {
  select_storage_medium(MEDIUM_CDROM, DEFAULT_MEDIUM);
 }
 
 //detect disk
 if(optical_drive_info.controller_type==AHCI_CONTROLLER) { //CDROM is connected to AHCI port
  status = satapi_detect_disk(optical_drive_info.base_1, optical_drive_info.base_2);
 }
 if(optical_drive_info.controller_type==IDE_CONTROLLER) { //CDROM is connected to IDE port
  status = patapi_detect_disk(optical_drive_info.base_1, optical_drive_info.base_2);
 }

 //read disk size
 if(status==STATUS_TRUE) {
  if(optical_drive_info.controller_type==AHCI_CONTROLLER) { //CDROM is connected to AHCI port
   satapi_read_capabilities(optical_drive_info.base_1, optical_drive_info.base_2);
  }
  if(optical_drive_info.controller_type==IDE_CONTROLLER) { //CDROM is connected to IDE port
   patapi_read_capabilities(optical_drive_info.base_1, optical_drive_info.base_2);
  }
 }
 
 //select previous medium
 if(save_value_storage_medium!=MEDIUM_CDROM || save_value_storage_medium_number!=DEFAULT_MEDIUM) {
  select_storage_medium(save_value_storage_medium, save_value_storage_medium_number);
 }
 
 //return status
 return status;
}

void eject_optical_disk(void) {
 byte_t status;
 
 //select medium
 if(if_storage_medium_exist(MEDIUM_CDROM, DEFAULT_MEDIUM)==STATUS_FALSE) {
  return;
 }
 
 save_value_storage_medium = storage_medium;
 save_value_storage_medium_number = storage_medium_number;
 
 if(storage_medium!=MEDIUM_CDROM) {
  select_storage_medium(MEDIUM_CDROM, DEFAULT_MEDIUM);
 }
 
 //eject drive
 if(optical_drive_info.controller_type==AHCI_CONTROLLER) { //CDROM is connected to AHCI port
  satapi_start_stop_command(optical_drive_info.base_1, optical_drive_info.base_2, PATA_EJECT);
 }
 if(optical_drive_info.controller_type==IDE_CONTROLLER) { //CDROM is connected to IDE port
  patapi_start_stop_command(optical_drive_info.base_1, optical_drive_info.base_2, PATA_EJECT);
 }
 
 //select previous medium
 if(save_value_storage_medium!=MEDIUM_CDROM || save_value_storage_medium_number!=DEFAULT_MEDIUM) {
  select_storage_medium(save_value_storage_medium, save_value_storage_medium_number);
 }

 //update device list
 device_list_check_optical_drive();
}

void spin_down_optical_drive(void) {
 if(optical_drive_info.controller_type==AHCI_CONTROLLER) { //CDROM is connected to AHCI port
  satapi_start_stop_command(optical_drive_info.base_1, optical_drive_info.base_2, PATA_SPIN_DOWN);
 }
 if(optical_drive_info.controller_type==IDE_CONTROLLER) { //CDROM is connected to IDE port
  patapi_start_stop_command(optical_drive_info.base_1, optical_drive_info.base_2, PATA_SPIN_DOWN);
 }
}

byte_t read_optical_disk_toc(void) {
 if(if_storage_medium_exist(MEDIUM_CDROM, DEFAULT_MEDIUM)==STATUS_FALSE) {
  return STATUS_ERROR;
 }

 //clear
 for(dword_t i=0; i<32; i++) {
  optical_disk_table_of_content.track[i].number = 0;
  optical_disk_table_of_content.track[i].first_sector = 0;
 }

 //read TOC
 if(optical_drive_info.controller_type==AHCI_CONTROLLER) { //CDROM is connected to AHCI port
  if(satapi_read_cd_toc(optical_drive_info.base_1, optical_drive_info.base_2, (dword_t)(&optical_disk_table_of_content))==STATUS_ERROR) {
   return STATUS_ERROR;
  }
 }
 if(optical_drive_info.controller_type==IDE_CONTROLLER) { //CDROM is connected to IDE port
  if(patapi_read_cd_toc(optical_drive_info.base_1, optical_drive_info.base_2, (dword_t)(&optical_disk_table_of_content))==STATUS_ERROR) {
   return STATUS_ERROR;
  }
 }

 //convert big endian to little endian
 optical_disk_table_of_content.length = BIG_ENDIAN(optical_disk_table_of_content.length);
 for(dword_t i=0; i<(optical_disk_table_of_content.last_track+1); i++) {
  byte_t byte_1 = (byte_t)(optical_disk_table_of_content.track[i].first_sector);
  byte_t byte_2 = (byte_t)(optical_disk_table_of_content.track[i].first_sector>>8);
  byte_t byte_3 = (byte_t)(optical_disk_table_of_content.track[i].first_sector>>16);
  byte_t byte_4 = (byte_t)(optical_disk_table_of_content.track[i].first_sector>>24);
  optical_disk_table_of_content.track[i].first_sector = (byte_1<<24 | byte_2<<16 | byte_3<<8 | byte_4);
 }
 return STATUS_GOOD;
}

void read_partition_info(void) {
 //clear partition table
 for(int i=0; i<8; i++) {
  partitions[i].type = STORAGE_NO_PARTITION;
  partitions[i].first_sector = 0;
  partitions[i].num_of_sectors = 0;
 }
 
 //clear partition name
 for(int i=0; i<11; i++) {
  partition_label[i]=0;
 }
 
 //we currently support only one partition on CDROM so it is handled in special way
 if(storage_medium==MEDIUM_CDROM) {
  if(detect_optical_disk()==STATUS_FALSE) {
   return;
  }

  if(is_partition_iso9660(0)==STATUS_TRUE) {
   partitions[0].type = STORAGE_ISO9660;
   partitions[0].first_sector = 0;
   partitions[0].num_of_sectors = optical_disk_size;
  }
  else if(is_optical_disk_cdda()==STATUS_TRUE) {
   partitions[0].type = STORAGE_CDDA;
   partitions[0].first_sector = 0;
   partitions[0].num_of_sectors = optical_disk_size;
  }
  else if(read_optical_disk_toc()==STATUS_TRUE && optical_disk_table_of_content.first_track==0) {
   partitions[0].type = STORAGE_FREE_SPACE;
   partitions[0].first_sector = 0;
   partitions[0].num_of_sectors = optical_disk_size;
  }
  else {
   return;
  }
  
  goto log;
 }
 
 //read sector with Master Boot Record
 if(read_storage_medium(0, 1, (dword_t)mbr_sector)==STATUS_ERROR) {
  return;
 }

 //Master Boot Record
 dword_t *mbr_ptr = (dword_t *) (((dword_t)mbr_sector)+0x1BE);
 if(mbr_ptr[3]!=0 || mbr_ptr[7]!=0 || mbr_ptr[11]!=0 || mbr_ptr[15]!=0) { //some info about partitions
  for(int i=0, partition_ptr=0; i<4; i++, mbr_ptr += 4) {
   if((mbr_ptr[1] & 0xFF)==0) { //free space
    if(mbr_ptr[3]==0) {
     continue; //free or invalid entry
    }
    partitions[partition_ptr].type = STORAGE_FREE_SPACE;
    partitions[partition_ptr].first_sector = mbr_ptr[2];
    partitions[partition_ptr].num_of_sectors = mbr_ptr[3];
    partition_ptr++;
   }
   else if((mbr_ptr[1] & 0xFF)==0x05 || (mbr_ptr[1] & 0xFF)==0x0F) {
    //read sector with Extended Boot Record
    if(read_storage_medium(mbr_ptr[2], 1, (dword_t)ebr_sector)==STATUS_ERROR) {
     continue;
    }

    //read partitions
    dword_t *ebr_ptr = (dword_t *) (((dword_t)ebr_sector)+0x1BE);
    for(dword_t j=0; j<2; j++, ebr_ptr += 4) {
     if((ebr_ptr[1] & 0xFF)==0) { //free space
      if(ebr_ptr[3]==0) {
       continue; //free or invalid entry
      }
      partitions[partition_ptr].type = STORAGE_FREE_SPACE;
      partitions[partition_ptr].first_sector = (mbr_ptr[2]+ebr_ptr[2]);
      partitions[partition_ptr].num_of_sectors = ebr_ptr[3];
      partition_ptr++;
     }
     else {
      //here is some filesystem
      partitions[partition_ptr].type = STORAGE_UNKNOWN_FILESYSTEM;
      partitions[partition_ptr].first_sector = (mbr_ptr[2]+ebr_ptr[2]);
      partitions[partition_ptr].num_of_sectors = ebr_ptr[3];
      partition_ptr++;
     }
    }
   }
   else if((mbr_ptr[1] & 0xFF)==0xEE) { //Global Partition Table
    dword_t gpt_memory = malloc(512*32);
    if(read_storage_medium(mbr_ptr[2], 32, gpt_memory)==STATUS_ERROR) { //read all Global Partition Table data
     free(gpt_memory);
     goto find_type_of_filesystem;
    }

    //read basic info
    dword_t *gpt_pointer = (dword_t *) (gpt_memory);
    dword_t sector_with_partition_table = gpt_pointer[18];
    dword_t number_of_partitions = gpt_pointer[20];
    dword_t size_of_partition_entry = gpt_pointer[21];
    if(gpt_pointer[19]!=0 || sector_with_partition_table==0 || sector_with_partition_table>2 || size_of_partition_entry<128 || (size_of_partition_entry%128)!=0) { //parition info is above 2 TB/invalid partition entry size
     goto find_type_of_filesystem;;
    }
    if(number_of_partitions>8) { //now we support max 8 partitions
     number_of_partitions = 8;
    }

    //read partition info
    if(sector_with_partition_table==1) {
     gpt_pointer = (dword_t *) (gpt_memory+0x5C);
    }
    else if(sector_with_partition_table==2) {
     gpt_pointer = (dword_t *) (gpt_memory+512);
    }
    for(dword_t j=0; j<number_of_partitions; j++, gpt_pointer=(dword_t *) (((dword_t)gpt_pointer)+size_of_partition_entry)) {
     //no partition
     if(gpt_pointer[0]==0 && gpt_pointer[1]==0 && gpt_pointer[2]==0 && gpt_pointer[3]==0) {
      break;
     }

     //parse partition info
     if(gpt_pointer[9]==0 && gpt_pointer[11]==0) { //we do not support partitions bigger than 2 TB
      partitions[partition_ptr].type = STORAGE_UNKNOWN_FILESYSTEM;
      partitions[partition_ptr].first_sector = gpt_pointer[8];
      partitions[partition_ptr].num_of_sectors = (gpt_pointer[10]-gpt_pointer[8]);
      partition_ptr++;
     }
    }

    free(gpt_memory);
    goto find_type_of_filesystem;
   }
   else {
    //here is some filesystem
    partitions[partition_ptr].type = STORAGE_UNKNOWN_FILESYSTEM;
    partitions[partition_ptr].first_sector = mbr_ptr[2];
    partitions[partition_ptr].num_of_sectors = mbr_ptr[3];
    partition_ptr++;
   }
  }
 }
 else { //no MBR
  partitions[0].type = STORAGE_UNKNOWN_FILESYSTEM;
  partitions[0].first_sector = 0;
  partitions[0].num_of_sectors = 0; //TODO:
 }
 
 //find type of filesystem
 find_type_of_filesystem:
 for(int i=0; i<8; i++) {
  if(partitions[i].type==STORAGE_UNKNOWN_FILESYSTEM) {
   if(is_partition_fat(partitions[i].first_sector)==STATUS_TRUE) {
    partitions[i].type = STORAGE_FAT;
   }
   else if(is_partition_ext(partitions[i].first_sector)==STATUS_TRUE) {
    partitions[i].type = STORAGE_EXT;
   }
   else if(is_partition_bleskos_bootable(partitions[i].first_sector)==STATUS_TRUE) {
    partitions[i].type = STORAGE_BLESKOS_BOOTABLE;
   }
  }
 }
 
 log:
 log("\npartitions founded:");
 for(int i=0; i<8; i++) {
  if(partitions[i].type!=STORAGE_NO_PARTITION) {
   log("\n");
   log_hex_with_space(partitions[i].first_sector);
   log_var_with_space(partitions[i].num_of_sectors);
   if(partitions[i].type==STORAGE_FAT) {
    log("FAT");
   }
   else if(partitions[i].type==STORAGE_EXT) {
    log("Ext");
   }
   else if(partitions[i].type==STORAGE_ISO9660) {
    log("ISO9660");
   }
   else if(partitions[i].type==STORAGE_CDDA) {
    log("CDDA");
   }
   else if(partitions[i].type==STORAGE_BLESKOS_BOOTABLE) {
    log("BleskOS bootable partition");
   }
   else {
    log("unknown filesystem");
   }
  }
 }
}

void select_partition(byte_t partition_number) {
 selected_partition_number = partition_number;
 
 //clear partition name
 for(int i=0; i<11; i++) {
  partition_label[i]=0;
 }

 if(partitions[partition_number].type==STORAGE_FAT) {
  select_fat_partition(partitions[partition_number].first_sector);
 }
 else if(partitions[partition_number].type==STORAGE_EXT) {
  select_ext_partition(partitions[partition_number].first_sector);
 }
 else if(partitions[partition_number].type==STORAGE_ISO9660) {
  select_iso9660_partition(partitions[partition_number].first_sector);
 }
 else if(partitions[partition_number].type==STORAGE_CDDA) {
  select_cdda_partition();
 }
}

byte_t is_filesystem_read_write(byte_t filesystem_type) {
 if(filesystem_type==STORAGE_FAT) {
  return STATUS_TRUE;
 }
 
 return STATUS_FALSE;
}

dword_t read_file(dword_t file_starting_entry, dword_t file_size) {
 if(partitions[selected_partition_number].type==STORAGE_FAT) {
  return read_fat_file(file_starting_entry, file_size);
 }
 else if(partitions[selected_partition_number].type==STORAGE_EXT) {
  return ext_read_file(file_starting_entry, file_size);
 }
 else if(partitions[selected_partition_number].type==STORAGE_ISO9660) {
  return iso9660_read_file(file_starting_entry, file_size);
 }
 else if(partitions[selected_partition_number].type==STORAGE_CDDA) {
  return cdda_read_file(file_starting_entry, file_size);
 }
 
 return STATUS_ERROR;
}

dword_t create_file(dword_t file_mem, dword_t size_of_file) {
 if(partitions[selected_partition_number].type==STORAGE_FAT) {
  return write_fat_file(file_mem, size_of_file, 0);
 }

 return STATUS_ERROR;
}

dword_t rewrite_file(dword_t file_starting_entry, dword_t file_size, dword_t file_mem, dword_t size_of_file) {
 if(partitions[selected_partition_number].type==STORAGE_FAT) {
  if(delete_fat_file(file_starting_entry)==STATUS_ERROR) {
   return STATUS_ERROR;
  }
  return write_fat_file(file_mem, size_of_file, file_starting_entry);
 }
 
 return STATUS_ERROR;
}

dword_t delete_file(dword_t file_starting_entry) {
 if(partitions[selected_partition_number].type==STORAGE_FAT) {
  return delete_fat_file(file_starting_entry);
 }
 
 return STATUS_ERROR;
}

dword_t read_folder(dword_t file_starting_entry, dword_t file_size) {
 if(partitions[selected_partition_number].type==STORAGE_FAT) {
  return read_fat_folder(file_starting_entry, file_size);
 }
 else if(partitions[selected_partition_number].type==STORAGE_EXT) {
  return ext_read_folder(file_starting_entry, file_size);
 }
 else if(partitions[selected_partition_number].type==STORAGE_ISO9660) {
  return read_iso9660_folder(file_starting_entry, file_size);
 }
 else if(partitions[selected_partition_number].type==STORAGE_CDDA) {
  return cdda_read_root_folder();
 }
 
 return STATUS_ERROR;
}

dword_t create_folder(dword_t folder_mem) {
 if(partitions[selected_partition_number].type==STORAGE_FAT) {
  dword_t fat_folder_mem = convert_folder_to_fat_folder(folder_mem);
  dword_t fat_cluster = write_fat_file(fat_folder_mem, fat_file_length, 0);
  free(fat_folder_mem);
  return fat_cluster;
 }
 
 return STATUS_ERROR;
}

dword_t rewrite_folder(dword_t folder_starting_entry, dword_t folder_mem) {
 if(partitions[selected_partition_number].type==STORAGE_FAT) {
  return rewrite_fat_folder(folder_starting_entry, folder_mem);
 }
 
 return STATUS_ERROR;
}