#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;


void getBinarization(Mat &image, Mat &gryimg, int threshold)
{
	int	width= image.rows; // number of lines in the image
	int	height= image.cols; // number of pixels per a line
	int channel = image.channels();
	
	Mat_<uchar> gimage, ggray;
	uchar* data = NULL;
	
	gimage=image;
	ggray=gryimg;
	
	for( int r=0; r<width; r++ )
	{
		if(channel == 3)
			data = gimage.ptr<uchar>( r );
		
		for( int c=0; c<height; c++)
		{
			if(channel == 1)
			{
				uchar &gv = gimage( r, c );
				ggray(r,c) = gv;
				if(gv > threshold) ggray(r,c) = 255;
	            else ggray(r,c) = 0;
			}
			else if(channel == 3)
			{
				uchar blue = data[3*c+0]; 
				uchar green = data[3*c+1]; 
				uchar red = data[3*c+2];
			    
	            uchar gv = (blue + green + red) / 3.0; 
	            
	            if(gv > threshold) ggray(r,c) = 255;
	            else ggray(r,c) = 0;

			}
		}
	}
}

void getBinarizationOtzu(Mat &image, Mat &gryimg)
{
	int	width= image.rows; // number of lines in the image
	int	height= image.cols; // number of pixels per a line
	int channel = image.channels();
	
	Mat_<uchar> gimage, ggray;
	uchar* data = NULL;
	
	gimage=image;
	ggray=gryimg;
	
	register int i,t;
	// Histogram 계산
	int	hist[256];
	float prob[256];
	
	for(i=0; i<256; i++) 
	{ 
		hist[i]=0; 
		prob[i] = 0.0f; 
	}
	
	// 영상의 histogram 계산
	for( int r=0; r<width; r++ )
	{
		if(channel == 3)
			data = gimage.ptr<uchar>( r );
		
		for( int c=0; c<height; c++)
		{
			if(channel == 1)
			{
				uchar &gv = gimage( r, c );
				ggray(r,c) = gv;
				hist[gv]++;
			}
			else if(channel == 3)
			{
				uchar blue = data[3*c+0]; 
				uchar green = data[3*c+1]; 
				uchar red = data[3*c+2];
			    
	            uchar gv = (blue + green + red) / 3.0; 
	            
	            ggray(r,c) = gv;
	            
	            hist[gv]++;
			}
		}
	}
	
	for(i=0; i<256; i++) 
		prob[i] = (float)hist[i]/(float)(height*width);
		
	float wsv_min= 1000000.0f;
	float wsv_u1, wsv_u2, wsv_s1, wsv_s2;
	int	wsv_t;
	
	for(t=0; t<256; t++)
	{
		// 누적확률q1, q2 계산
		float q1 = 0.0f, q2 = 0.0f;
		
		for(i=0; i<t; i++) q1 += prob[i];
			
		for(i=t; i<256; i++) q2 += prob[i];
			
		if(q1==0 || q2==0) continue;
		
		// 평균u1, u2 계산
		float u1=0.0f, u2=0.0f;
		
		for(i=0; i<t; i++) u1 += i*prob[i]; 
		u1 /= q1;
		
		for(i=t; i<256; i++) u2 += i*prob[i]; 
		u2 /= q2;
		
		// 분산s1, s2 계산
		float s1=0.0f, s2=0.0f;
		for(i=0; i<t; i++) s1 += (i-u1)*(i-u1)*prob[i]; 
		s1 /= q1;
		for(i=t; i<256; i++) s2 += (i-u2)*(i-u2)*prob[i]; 
		s2 /= q2;
		
		//총분산계산
		float wsv= q1*s1+q2*s2;
		
		if(wsv< wsv_min)
		{
			wsv_min= wsv;
			wsv_t= t; // 최소치저장
			wsv_u1 = u1; wsv_u2 = u2;
			wsv_s1 = s1; wsv_s2 = s2;
		}
	}
	
	// thresholding
	for( int r=0; r<width; r++ )
	{
		if(channel == 3)
			data = gimage.ptr<uchar>( r );
		
		for( int c=0; c<height; c++)
		{
			if(channel == 1)
			{
				uchar &gv = gimage( r, c );
				ggray(r,c) = gv;
				
				
				if(gv > wsv_t) ggray(r,c) = 255;
	            else ggray(r,c) = 0;
			}
			else if(channel == 3)
			{
				uchar blue = data[3*c+0]; 
				uchar green = data[3*c+1]; 
				uchar red = data[3*c+2];
			    
	            uchar gv = (blue + green + red) / 3.0; 
	            
	            if(gv > wsv_t) ggray(r,c) = 255;
	            else ggray(r,c) = 0;

			}
		}
	}
	
}

void getHistoEqual(Mat &image, Mat &gryimg)
{
	int	i;
	int	numOfLines= image.rows; // number of lines in the image
	int	numOfPixels= image.cols; // number of pixels per a line
	int channel = image.channels();
	/// histogram연산을 위해 사용할 배열 메모리를 할당
	unsigned int* histogram = new unsigned int[256];
	unsigned int* sum_hist= new unsigned int[256];
	
	Mat_<uchar> gimage, ggray;
	uchar* data = NULL;

	/// histogram배열을초기화
	for(i=0; i<256; i++) histogram[i]=0;

	gimage=image;
	ggray=gryimg;
	
	// 영상의histogram을계산
	for( int r=0; r<numOfLines; r++ )
	{
		if(channel == 3)
			data = gimage.ptr<uchar>( r );
		
		for( int c=0; c<numOfPixels; c++)
		{
			if(channel == 1)
			{
				uchar &gv = gimage( r, c );
				ggray(r,c) = gv;
				histogram[gv]++;
			}
			else if(channel == 3)
			{
				uchar blue = data[3*c+0]; 
				uchar green = data[3*c+1]; 
				uchar red = data[3*c+2];
			    
	            uchar gv = (blue + green + red) / 3.0; 
	            
	            ggray(r,c) = gv;
	            
	            histogram[gv]++;
			}
		}
	}

	/// histogram의정규화된합을계산
	//값 0 ~ 255 사이
	int	sum=0;
	float scale_factor=255.0f/(float)(numOfLines*numOfPixels);
	
	for(i=0; i<256; i++)
	{
		sum += histogram[i];
		sum_hist[i] =(int)((sum*scale_factor) + 0.5); //반올림
	}
	
	/// LUT로써sum_hist배열을사용하여영상을변환
	for( int r=0; r<image.rows; r++ )
	{
		if(channel == 3)
			data = image.ptr<uchar>( r );
			
		for( int c=0; c<numOfPixels; c++)
		{
			if(channel == 1)
			{
				uchar &gv = gimage( r, c );
				//ghisto(r,c) = sum_hist[gv];
				ggray(r,c) = sum_hist[gv];
			}
			else if(channel == 3)
			{
				uchar blue = data[3*c+0]; 
				uchar green = data[3*c+1]; 
				uchar red = data[3*c+2]; 
			    
	            uchar gv = (blue + green + red) / 3.0; 
	            
	            //ghisto(r,c) = sum_hist[gv];
	            ggray(r,c) = sum_hist[gv];
			}
		}
	}
	// 메모리 해제
	delete []histogram; delete []sum_hist;
}

void getHistoUpStretch(Mat &image, Mat &gryimg, int lowPercent, int highPercent)
{
	int i;
	int	numOfLines= image.rows; // number of lines in the image
	int	numOfPixels= image.cols; // number of pixels per a line
	int channel = image.channels();
	
	// histogram연산을위해사용할배열을할당
	unsigned int* histogram = new unsigned int[256];
	
	Mat_<uchar> gimage, ggray;
	uchar* data = NULL;
	
	/// histogram배열을초기화
	for(i=0; i<256; i++) histogram[i]=0;

	gimage=image;
	ggray=gryimg;
	
	// 영상의 histogram 계산
	for( int r=0; r<numOfLines; r++ )
	{
		if(channel == 3)
			data = gimage.ptr<uchar>( r );
		
		for( int c=0; c<numOfPixels; c++)
		{
			if(channel == 1)
			{
				uchar &gv = gimage( r, c );
				ggray(r,c) = gv;
				histogram[gv]++;
			}
			else if(channel == 3)
			{
				uchar blue = data[3*c+0]; 
				uchar green = data[3*c+1]; 
				uchar red = data[3*c+2];
			    
	            uchar gv = (blue + green + red) / 3.0; 
	            ggray(r,c) = gv;
	            
	            histogram[gv]++;
			}
		}
	}
	
	//
	unsigned int runsum=0;
	int lowthresh=0, highthresh=255;
	
	for(i=0; i<256; i++)
	{
		runsum+= histogram[i];
		if ( (runsum*100.0/(float)(numOfLines*numOfPixels)) >= lowPercent)
		{
			lowthresh=i;
			break;
		}
	}
	
	runsum=0;
	
	for(i=255; i>=0; i--)
	{
		runsum+= histogram[i];
		if ( (runsum*100.0/(float)(numOfLines*numOfPixels)) >= highPercent)
		{
			highthresh=i;
			break;
		}
	}
	
	// 새로운LUT를계산함
	unsigned char *LUT=new unsigned char [256];
	
	for(i=0; i<lowthresh; i++) LUT[i]=0;
	
	for(i=255; i>highthresh; i--) LUT[i]=255;
	
	float scale = 255.0f/(float)(highthresh-lowthresh);
	for(i=lowthresh; i<=highthresh; i++)
		LUT[i] = (unsigned char)((i-lowthresh) * scale);
		
	// LUT로써sum_hist배열을 사용한 영상 변환
	/// LUT로써sum_hist배열을사용하여영상을변환
	for( int r=0; r<image.rows; r++ )
	{
		if(channel == 3)
			data = image.ptr<uchar>( r );
			
		for( int c=0; c<numOfPixels; c++)
		{
			if(channel == 1)
			{
				uchar &gv = gimage( r, c );
				//ghisto(r,c) = sum_hist[gv];
				ggray(r,c) = LUT[gv];
			}
			else if(channel == 3)
			{
				uchar blue = data[3*c+0]; 
				uchar green = data[3*c+1]; 
				uchar red = data[3*c+2]; 
			    
	            uchar gv = (blue + green + red) / 3.0; 
	            
	            //ghisto(r,c) = sum_hist[gv];
	            ggray(r,c) = LUT[gv];
			}
		}
	}
	
	// 메모리를해제
	delete []histogram; delete []LUT;
}

void getFlexHistogram(Mat &image, Mat &histo)
{
	int histoArr[256];
	Mat_<uchar> gimage, ghisto;
	
	gimage=image;
	ghisto=histo;
	
	int i,j,vmax,vmin;
	
	for(i=0; i<256; i++) histoArr[i]=0;
	
	for( int r=0; r<256; r++ )
	{
		for( int c=0; c<256; c++)
		{
			uchar &gv = gimage( r, c );
		
			histoArr[gv]++;
		}
	}
	
	// 히스토그램크기정규화
	vmin= 1000000; vmax=0;
	
	for(i=0; i<256; i++)
	{
		if(histoArr[i]<=vmin) vmin= histoArr[i];
			
		if(histoArr[i]>=vmax) vmax= histoArr[i];
	}
	
	if(vmax==vmin) return;
		
	float vd= (float)(vmax-vmin);
	
	for(i=0; i<256; i++)
	{
		histoArr[i] = (int)( ((float)histoArr[i]-vmin)*255.0/vd); // 최대값을255 로제한
	}
	
	// 히스토그램의출력
	for( int r=0; r<256; r++ )
	{
		for( int c=0; c<256; c++)
		{
			ghisto( r, c ) = 255; // 그래프바탕은흰색으로
		}
	}
	
	for(j=0; j<256; j++)
	{
		for(i=0; i<histoArr[j]; i++)
		{
			ghisto( 255-i, j ) = 0;
		}
	}
	
	histo = ghisto;
}


int	main( void )
{
	Mat orgImage, grayImg, histoImg;
	
	//orgImage = imread("../images/fruit-grayscale.bmp");
	//orgImage = imread("../images/fruites.bmp");
	//orgImage = imread("../images/lena_gray.bmp");
	//orgImage = imread("../images/bin_sample.png");
	//orgImage = imread("../images/bin_sample2.jpg");
	orgImage = imread("./images/flor-flower-garden-736230.jpg");
	
	grayImg = Mat(orgImage.rows, orgImage.cols, CV_8UC1);
	imshow("image original", orgImage);
	histoImg = Mat(256, 256, CV_8UC1);
	
	//1. 히스토그램 화
	getFlexHistogram(orgImage, histoImg);
	imshow("original histogram", histoImg);
	
	//2. 히스토그램 정규화
	getHistoEqual(orgImage, grayImg);
	getFlexHistogram(grayImg, histoImg);
	
	imshow("image eqaulization", grayImg);
	imshow("eqaulization histogram2", histoImg);
	
	//3. 히스토그램 스트레칭
	int lowPercent = 50;
	int highPercent = 5;
	
	getHistoUpStretch(orgImage, grayImg, lowPercent, highPercent);
	getFlexHistogram(grayImg, histoImg);
	imshow("image stretch", grayImg);
	imshow("stretch histogram", histoImg);

	
	//4. 이진화 - 일반
	int threshold = 100;
	getBinarization(orgImage, grayImg, threshold);
	imshow("image binarization", grayImg);

	
	//5. 이진화 - Otzu
	getBinarizationOtzu(orgImage, grayImg);
	imshow("Otzu binarization", grayImg);
	
	waitKey(0);
}

