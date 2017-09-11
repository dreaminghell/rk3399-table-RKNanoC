/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   AddrSaveMacro.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*               anzhiguo      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef PINYIN_SORT_H
#define PINYIN_SORT_H
//system reserved area(sector) size.
#define 	SECTOR_BYTE_SIZE            512	    /*byte number in one sector */

#define 	BYTE_NUM_SAVE_PER_FILE 	    512	    /*occupy byte number each file information*/
#define     MEDIAINFO_PAGE_SIZE         (16*1024)
#define 	FILE_SAVE_NUM_PER_PAGE 	    MEDIAINFO_PAGE_SIZE/BYTE_NUM_SAVE_PER_FILE/* 32,save file information,the number that one page can save in each page*/
#define 	SECTOR_BYTE_SIZE            512	    /*the byte number of one sector. */

//shift bit number it is use to get physical address from sector address.
#define 	BYTE_PER_FILE_SHIFT 		9	/*once left shift BYTE_PER_FILE_SHIFT bits is equal with BYTE_NUM_SAVE_PER_FILE */
/*
--------------------------------------------------------------------------------
 the memory allocate of one secector to save file deteiled information;
 	offset address          save information        occupy size(unit : byte)
        0                   long file namge         FILE_NAME_SAVE_CHAR_NUM*2 (80)
        80                  ID3_TITLE               CHAR_NUM_PER_ID3_TITLE*2  (60)
     80+60=140              ID3_SINGLE              CHAR_NUM_PER_ID3_SINGLE*2 (60)
    80+60+60=200            ID3_ALBUM               CHAR_NUM_PER_ID3_ALBUM*2  (60)
   80+60+60+60=260          file path info          (3 + (MAX_DIR_DEPTH - 1) * 12 + 1)(40)
  80+60+60+60+40=300        short file name           11
  
                                                    total : 311 byte
History:     <author>         <time>         <version>       
            anzhiguo     2009-6-4         Ver1.0 
--------------------------------------------------------------------------------
*/ 

#define   MEDIA_CHAR_NUM_PER_FILE_NAME 	    FILE_NAME_SAVE_CHAR_NUM	//the length to keep one file name(16bit) wchar type.
#define   FILE_NAME_SAVE_ADDR_OFFSET    0 /*In each file info keeping space, file name offset is the start postion in each keeping file infomation section.*/
#define   FILE_NAME_SAVE_SIZE           (MEDIA_CHAR_NUM_PER_FILE_NAME*2)   /*occupy byte number to save one file name */

#define   CHAR_NUM_PER_ID3_TITLE        MEDIA_ID3_SAVE_CHAR_NUM	    /*length of one ID3_TITLE(16bit)  */
#define   ID3_TITLE_SAVE_ADDR_OFFSET    (FILE_NAME_SAVE_ADDR_OFFSET+FILE_NAME_SAVE_SIZE) /*keeping ID3_TITLE offset in file information section */
#define   ID3_TITLE_SAVE_SIZE           (CHAR_NUM_PER_ID3_TITLE*2)            /*the byte number of one save ID3_TITLE */

#define   CHAR_NUM_PER_ID3_SINGLE 	    MEDIA_ID3_SAVE_CHAR_NUM	/* length of one ID3_SINGLE(16bit) */
#define   ID3_SINGLE_SAVE_ADDR_OFFSET   (ID3_TITLE_SAVE_ADDR_OFFSET+ID3_TITLE_SAVE_SIZE) /*keeping ID3_SINGLE offset in file information section */
#define   ID3_SINGLE_SAVE_SIZE          (CHAR_NUM_PER_ID3_SINGLE*2)          /*the byte number of one save ID3_SINGLE*/

#define   CHAR_NUM_PER_ID3_ALBUM 	    MEDIA_ID3_SAVE_CHAR_NUM	    /*length of one ID3_ALBUM(16bit) */
#define   ID3_ALBUM_SAVE_ADDR_OFFSET    (ID3_SINGLE_SAVE_ADDR_OFFSET+ID3_SINGLE_SAVE_SIZE) /*keeping ID3_ALBUM offset in file information section */
#define   ID3_ALBUM_SAVE_SIZE           (CHAR_NUM_PER_ID3_ALBUM*2)            /*the byte number of one save ID3_ALBUM*/

#define   CHAR_NUM_PER_ID3_GENRE 	    MEDIA_ID3_SAVE_CHAR_NUM	    /*length of one ID3_GENRE(16bit) */
#define   ID3_GENRE_SAVE_ADDR_OFFSET    (ID3_ALBUM_SAVE_ADDR_OFFSET+ID3_ALBUM_SAVE_SIZE) /*keeping ID3_GENRE offset in file information section*/
#define   ID3_GENRE_SAVE_SIZE           (CHAR_NUM_PER_ID3_GENRE*2)            /*the byte number of one save ID3_GENRE*/

#define   PATH_SAVE_ADDR_OFFSET         (ID3_GENRE_SAVE_ADDR_OFFSET+ID3_GENRE_SAVE_SIZE) /*keeping path name offset in file information section*/
#define   PATH_SAVE_SIZE                ( 3 + (MAX_DIR_DEPTH - 1) * 12 + 1)   /*the byte number of one save path*/

#define   SHORT_NAME_SAVE_ADDR_OFFSET   (PATH_SAVE_ADDR_OFFSET + PATH_SAVE_SIZE) /*keeping short file name offset in file information section*/
#define   SHORT_NAME_SAVE_SIZE          11                                   /*the byte number of one save short file name*/

#define   FILENUM_SAVE_ADDR_OFFSET   (SHORT_NAME_SAVE_ADDR_OFFSET + SHORT_NAME_SAVE_SIZE) /*keeping file number offset in file information section*/
#define   FILENUM_SAVE_SIZE          4    

/*
*************************************************************************************************************
    Meida Lib Address map
   StartAddress: 0000   -----------------------------------
                        |                                 |
                        |   FILE_SAVE_INFO(2048*4)        |
                        |                                 |
   StartAddress: 2048*4 -----------------------------------
                        |                                 |
                        |   FILE_SORT_INFO(2048*4)        |
                        |                                 |
   StartAddress: 2048*8 -----------------------------------
                        |                                 |
                        |   FAVORITE_INFO(2048*4*2)       |
                        |                                 |
   StartAddress: 2048*16-----------------------------------
    
*************************************************************************************************************
*/
//------------------------------------------------------------------------
//File Save Info (Addr: 0)
#define		FILE_SAVE_INFO_SECTOR_START		    0 /*the start address to save file detailed information */
#define		FILE_SAVE_SECTOR_SIZE		        (2048*4) /*file information occupy memory, 2048k is 2M */

//------------------------------------------------------------------------
//File Sort info (Addr: 2048*4)
#define		FILENAME_SORT_INFO_SECTOR_START		(FILE_SAVE_INFO_SECTOR_START+FILE_SAVE_SECTOR_SIZE) /*the saved start address of file name sort information */
#define     FILENAME_SORT_INFO_SIZE		        32  /*file name sort information occupy memory,16 sec that is 8k */

#define		ID3TITLE_SORT_INFO_SECTOR_START		(FILENAME_SORT_INFO_SECTOR_START+FILENAME_SORT_INFO_SIZE) /*the start sector address of ID3_TITLE sort information*/
#define     ID3TITLE_SORT_INFO_SIZE		        32  

#define		ID3ARTIST_SORT_INFO_SECTOR_START	(ID3TITLE_SORT_INFO_SECTOR_START+ID3TITLE_SORT_INFO_SIZE) /*the start sector address of ID3_ARTIST sort information*/
#define     ID3ARTIST_SORT_INFO_SIZE		    32

#define		ID3ARTIST_SORT_SUB_SECTOR_START		(ID3ARTIST_SORT_INFO_SECTOR_START+ID3ARTIST_SORT_INFO_SIZE) /*the start sector address of ID3_ARTIST classified information*/
#define     ID3ARTIST_SORT_SUB_SIZE		        128  /* ID3_ARTIST classified information occupy 64 sec that is 32k*/ 

#define		ID3ALBUM_SORT_INFO_SECTOR_START		(ID3ARTIST_SORT_SUB_SECTOR_START+ID3ARTIST_SORT_SUB_SIZE) /*the start sector address of ID3_ALBUM sort information*/
#define     ID3ALBUM_SORT_INFO_SIZE		        32  

#define		ID3ALBUM_SORT_SUB_SECTOR_START		(ID3ALBUM_SORT_INFO_SECTOR_START+ID3ALBUM_SORT_INFO_SIZE) /*the start sector address of ID3_ALBUM classified information*/
#define     ID3ALBUM_SORT_SUB_SIZE		        128  

#define		ID3GENRE_SORT_INFO_SECTOR_START		(ID3ALBUM_SORT_SUB_SECTOR_START+ID3ALBUM_SORT_SUB_SIZE) /*the start sector address of ID3_GENRE sort information*/
#define     ID3GENRE_SORT_INFO_SIZE		        32  

#define		ID3GENRE_SORT_SUB_SECTOR_START		(ID3GENRE_SORT_INFO_SECTOR_START+ID3GENRE_SORT_INFO_SIZE) /*the start sector address of ID3_GENRE classified information*/
#define     ID3GENRE_SORT_SUB_SIZE		        128  

//------------------------------------------------------------------------
//Favorite info (Addr: 2048*8)
#define		FAVORITE_MUSIC_INFO_SECTOR_START	(2048*8)    /*start address of the keeping favorite infomation*/
#define		FAVORITE_MUSIC_SECTOR_SIZE		    (2048*4)    /*the memory size of favorite 2000 sectoin is equal with 1M*/
#define		FAVORITE_BLOCK_SECTOR_START1        (FAVORITE_MUSIC_INFO_SECTOR_START)
#define		FAVORITE_BLOCK_SECTOR_START2        (FAVORITE_MUSIC_INFO_SECTOR_START+FAVORITE_MUSIC_SECTOR_SIZE)

#define     MEDIAFILE_INFO_END_ADD              (FAVORITE_BLOCK_SECTOR_START2+FAVORITE_MUSIC_SECTOR_SIZE)    
/*
*************************************************************************************************************

    Meida Lib Address map End   
    
*************************************************************************************************************
*/

/*internal offset definition in favorite info */
#define   FAVORITE_MUSIC_SAVE_SIZE              128   /*the occupy byte number of each favourite */
#define   FAVORITE_NUM_PER_PAGE                 64   /*one page can save 128 file informations,as the size of buffer that write to flash is 8k, so write 128 file path informatin to flahs every time */
#define   FAVORITE_MUSIC_PATH_OFFSET            0
#define   FAVORITE_MUSIC_PATH_SIZE              PATH_SAVE_SIZE
#define   FAVORITE_MUSIC_NAME_OFFSET            (FAVORITE_MUSIC_PATH_OFFSET+FAVORITE_MUSIC_PATH_SIZE) 
#define   FAVORITE_MUSIC_NAME_SIZE              SHORT_NAME_SAVE_SIZE

/*internal offset definition in sort info */
#define   SUB_FULLINFO_START_OFFSET             0
#define   SUB_SORT_START_ID_OFFSET              2
#define   SUB_INCLUDE_ITEMNUM_OFFSET            4

#define   SUB_PER_ITEM_SIZE                     8
#endif/*PINYIN_SORT_H*/



