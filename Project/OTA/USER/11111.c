//if(f_mount(DRIVE_USBHOST,&fs_d)==FR_OK)     //װ��SD��	
//					{	
//						f_chdrive(DRIVE_USBHOST);	   //����ΪSD��  
//						
//						for(i=0;i<totalitemcnt;i++)
//						{
//							if(!fileinfolist[i].bDir)
//							{ 
//								if(font)
//								{  
//									drawsetrgb(0,0,0);
//									drawrectfill(30,170,450,20);//drawrectfill(50,170,700,20);    //��� �ļ���
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
//								if(FR_OK == f_open(&f,fileinfolist[i].fname,FA_READ|FA_OPEN_EXISTING)) //����������,����Boot.bin ����
//								{
//									//bmfont_write(50,60,"boot.bin is updating..... ");  
//									res=f_mount(DRIVE_SERIALFLASH,&fs);     //ע�� Spi Flash
//									res=f_chdrive(DRIVE_SERIALFLASH);       //����Ŀ¼Ϊ Spi FLASH
//									
//									WatchDog();
//									
//									res=f_open(&f_new,fileinfolist[i].fname,FA_WRITE|FA_CREATE_ALWAYS);  //��Boot.bin д������  
//									if(res==FR_OK)  //�������ܴ�,��ʼ����
//									{ 
//										fsize= f_size(&f);   //��ȡ�ļ�����
//										if(fsize>64*1024)     //����64K��ʼ�ֶ�
//										{
//											write_size = 64 * 1024;
//										}
//										else
//										{
//											write_size = fsize;
//										} 
//										
//										buf=malloc(write_size);  //�����ڴ�
//										if(buf==0)	
//											debugprintf("malloc bmpbuf error! \r\n");
//										
//										WatchDog();
//							      д���жϴ�			
//										while(fsize>write_size)
//										{	 
//											res=f_read(&f,buf,write_size,&nRead);  //��ȡ64K�ļ�  
//											res=f_write(&f_new,buf,write_size,&nWrite);
//											WatchDog();
//											fsize-=write_size;
//										} 
//										
//										if(fsize>0)
//										{
//											res = f_read(&f,buf,fsize,&nRead);	//Դͷ����512�ֽ�					
//											res = f_write(&f_new,buf,fsize,&nWrite);//д��Ŀ���ļ�
//										}
//										
//										// res=f_mount(DRIVE_SERIALFLASH,NULL);
//										free(buf); 
//									}
//									
//									f_close(&f_new);   //�ر��ļ�
//								}
//								f_close(&f);       //�ر��ļ�
//								
//								WatchDog();
//								
//								res=f_mount(DRIVE_USBHOST,&fs_d);
//								res=f_chdrive(DRIVE_USBHOST);	   //����ΪSD�� 
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
//								drawrectfill(150,230,200,20);//drawrectfill(350,230,200,20);   //��� �ٷֱ�
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