//if(f_mount(DRIVE_USBHOST,&fs_d)==FR_OK)     //装载SD卡	
//					{	
//						f_chdrive(DRIVE_USBHOST);	   //更改为SD卡  
//						
//						for(i=0;i<totalitemcnt;i++)
//						{
//							if(!fileinfolist[i].bDir)
//							{ 
//								if(font)
//								{  
//									drawsetrgb(0,0,0);
//									drawrectfill(30,170,450,20);//drawrectfill(50,170,700,20);    //清除 文件名
//									WatchDog();
//									
//									sprintf(percent,"Copying: %s",fileinfolist[i].fname);
//									bitfont_draw(g_pFont,30,170, percent);//bitfont_draw(g_pFont,50,170, percent);	
//									
//									//per=(i+1)*100/totalitemcnt;
//									//sprintf(percent,"%d%%",per);
//									//bitfont_draw_vright(g_pFont, 180,125, percent);
//									drawsetrgb(255,255,255);
//								}
//								
//								WatchDog();
//								
//								if(FR_OK == f_open(&f,fileinfolist[i].fname,FA_READ|FA_OPEN_EXISTING)) //能正常打开则,更换Boot.bin 程序
//								{
//									//bmfont_write(50,60,"boot.bin is updating..... ");  
//									res=f_mount(DRIVE_SERIALFLASH,&fs);     //注册 Spi Flash
//									res=f_chdrive(DRIVE_SERIALFLASH);       //更改目录为 Spi FLASH
//									
//									WatchDog();
//									
//									res=f_open(&f_new,fileinfolist[i].fname,FA_WRITE|FA_CREATE_ALWAYS);  //打开Boot.bin 写并覆盖  
//									if(res==FR_OK)  //两个都能打开,则开始复制
//									{ 
//										fsize= f_size(&f);   //获取文件长度
//										if(fsize>64*1024)     //大于64K开始分段
//										{
//											write_size = 64 * 1024;
//										}
//										else
//										{
//											write_size = fsize;
//										} 
//										
//										buf=malloc(write_size);  //申请内存
//										if(buf==0)	
//											debugprintf("malloc bmpbuf error! \r\n");
//										
//										WatchDog();
//							      写入判断处			
//										while(fsize>write_size)
//										{	 
//											res=f_read(&f,buf,write_size,&nRead);  //读取64K文件  
//											res=f_write(&f_new,buf,write_size,&nWrite);
//											WatchDog();
//											fsize-=write_size;
//										} 
//										
//										if(fsize>0)
//										{
//											res = f_read(&f,buf,fsize,&nRead);	//源头读出512字节					
//											res = f_write(&f_new,buf,fsize,&nWrite);//写入目的文件
//										}
//										
//										// res=f_mount(DRIVE_SERIALFLASH,NULL);
//										free(buf); 
//									}
//									
//									f_close(&f_new);   //关闭文件
//								}
//								f_close(&f);       //关闭文件
//								
//								WatchDog();
//								
//								res=f_mount(DRIVE_USBHOST,&fs_d);
//								res=f_chdrive(DRIVE_USBHOST);	   //更改为SD卡 
//							}
//							//----------------------------------
//							per=(i+1)*450/totalitemcnt;
//							drawrectfill(30,200,per,20);//drawrectfill(50,200,per,20);
//							
//							WatchDog();
//							
//							if(font)
//							{
//								drawsetrgb(0,0,0);
//								drawrectfill(150,230,200,20);//drawrectfill(350,230,200,20);   //清除 百分比
//								drawsetrgb(255,255,255);	
//							
//								per=(i+1)*100/totalitemcnt;
//								
//								sprintf(percent,"%d%%",per);
//								
//								bitfont_draw(g_pFont,150,230, percent);//bitfont_draw(g_pFont,390,230, percent);
//							}
//							WatchDog();
//						}
//					}
//			    }