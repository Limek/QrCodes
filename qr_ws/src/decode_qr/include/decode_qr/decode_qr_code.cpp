#include "decode_qr_code.h"
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;
using namespace zbar;

void code_qr::fun(int a){
	a = a*2;
	cout<<a<<endl;
}

double code_qr::angle(cv::Point pt1, cv::Point pt2, cv::Point pt0){
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void code_qr::decoder(Mat& img_qr, string& message){
	ImageScanner scanner;
      	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	Mat img;
	img_qr.copyTo(img);
	//if( !img.data ){
          	//return 0;
      	//}
	Mat imgout;
      	cvtColor(img,imgout,CV_GRAY2RGB);
      	int width = img.cols;
      	int height = img.rows;
   	uchar *raw = (uchar *)img.data;
	Image image(width, height, "Y800", raw, width * height);
	int n = scanner.scan(image);
	for(Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
		message = symbol->get_data();	     	
		//cout << "decoded " << symbol->get_type_name()<< " symbol \"" << symbol->get_data() << '"' <<" "<< endl;
		
	}
	image.set_data(NULL, 0);

}

void code_qr::detect_squares(){
	vector<Point> approx;	
	
	for (size_t i = 0; i < contours.size(); i++)
	{
		
	    	approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true); // robi przyblizenie do wielokata 

	    	if (approx.size() == 4 && // 4 wierzcholki 
			fabs(contourArea(Mat(approx))) > 1 && // powierzchnia wieksza niz 1000
			isContourConvex(Mat(approx))) // sprawdza czy wielokat jest wypukly
	    	{
		double maxCosine = 0;

		for( int j = 2; j < 5; j++ )
		{
		    	double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
		    	maxCosine = MAX(maxCosine, cosine);
		}
		vector<vector<Point> > contours3;
		vector<Vec4i> hierarchy3;
		if( maxCosine < 0.3 ) {
			RotatedRect r = minAreaRect(contours[i]);
			//RotatedRect re = minAreaRect(approx);
			Size r_size = r.size;
			Mat M, rotated, cropped;
        		float angled = r.angle;
        		//Size rect_size = rect.size;
        		if (r.angle < -45.) {
           			angled += 90.0;
            			swap(r_size.width, r_size.height);
       	 		}
        		M = getRotationMatrix2D(r.center, angled, 1.0);
        		warpAffine(pic, rotated, M, pic.size(), INTER_CUBIC);
        		getRectSubPix(rotated, r_size, r.center, cropped);
			Mat canny_out3;			
			Canny(cropped,canny_out3,100,200);
			
			findContours(canny_out3,contours3,hierarchy3,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);
			drawContours(cropped, contours3, -1, Scalar(255,0,0), 3, 8);
			cout<<contours3.size()<<endl;
			imshow("test",cropped);
			waitKey(0);
			vector<Point> approx3;
			int cnt=0;
			
			if(!contours3.empty()){
				for( int k=0; k<contours3.size(); k++){
					approxPolyDP(Mat(contours3[k]), approx3, arcLength(Mat(contours3[k]), true)*0.02, true);
					if (approx3.size() == 4 && // 4 wierzcholki 
						fabs(contourArea(Mat(approx3))) > 1 && // powierzchnia wieksza niz 1000
						isContourConvex(Mat(approx3))) // sprawdza czy wielokat jest wypukly
		    			{
						double maxCosine3 = 0;
						//cout<<"hdhusduhdshuds"<<endl;
						for( int l = 2; l < 5; l++ )
						{
			    				double cosine = fabs(angle(approx3[l%4], approx3[l-2], approx3[l-1]));
			    				maxCosine3 = MAX(maxCosine3, cosine);
						}
						if( maxCosine3 < 0.3 ) {
							cnt++;
						}
					}
		
				}
			}
			//imshow("test",cropped);
			//waitKey(0);
			cout<<cnt<<endl;
			if((double)r_size.width/(double)r_size.height > 0.8 && (double)r_size.width/(double)r_size.height < 1.2 && cnt < 5){// && cnt > 1){
				squares_contours.push_back(approx);
			}
		    	//squares_contours.push_back(approx);
		}
	    	}
	}
}

void code_qr::cut_qr(Mat& img_all){

for(int i=0; i<squares_contours.size(); i++){
	



}





}

void code_qr::add_contour(vector<Point>& contour){
	contours.push_back(contour);

}

void code_qr::detect_the_chosen_one(){

vector<Point> approx;
vector<vector<Point> > approx_chosen;

	for (size_t i = 0; i < contours.size(); i++)
	{
	    	approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
		//cout<<"Cont "<<i<<endl;

	    if (approx.size() == 4 &&
		fabs(contourArea(Mat(approx))) > 100 &&
		isContourConvex(Mat(approx)))
	    {
	
		approx_chosen.push_back(approx);

		//for (int j = 0; j < 4; j++)
		//	line(pic, approx[j], approx[(j+1)%4], Scalar(0,0,0));
		//cout<<approx<<endl;
		//imshow("Approx",pic);
		//waitKey(0);

	    }
	}
	int maxAreaApprox = 0;
	int maxAreaApproxInx = 0;
	for (int i=0; i<approx_chosen.size(); i++){
		if(maxAreaApprox < fabs(contourArea(Mat(approx_chosen[i])))){
			maxAreaApprox = fabs(contourArea(Mat(approx_chosen[i]))); 
			maxAreaApproxInx = i;	
		}
		
	}
	Mat perspQr;
	Mat perspT;
	Point2f src[4];
	Point2f dst[4];

	dst[0].x=0;
	dst[0].y=0;

	dst[1].x=300;
	dst[1].y=0;

	dst[2].x=300;
	dst[2].y=300;

	dst[3].x=0;
	dst[3].y=300;
	
	/*dst[0].x=0;
	dst[0].y=0;

	dst[1].x=0;
	dst[1].y=300;

	dst[2].x=300;
	dst[2].y=300;

	dst[3].x=300;
	dst[3].y=0;
	
	
	src[0].x=approx_chosen[maxAreaApproxInx][0].x;
	src[0].y=approx_chosen[maxAreaApproxInx][0].y;

	src[1].x=approx_chosen[maxAreaApproxInx][1].x;
	src[1].y=approx_chosen[maxAreaApproxInx][1].y;

	src[2].x=approx_chosen[maxAreaApproxInx][2].x;
	src[2].y=approx_chosen[maxAreaApproxInx][2].y;

	src[3].x=approx_chosen[maxAreaApproxInx][3].x;
	src[3].y=approx_chosen[maxAreaApproxInx][3].y;*/

	bool p[4];
	bool ap[4];
	for (int i=0; i<4; i++){
		p[i] = 0;
		ap[i] = 0;	
	}
	int max_x=0;
	int min_x=10000;	
	int max_y=0;
	int min_y=10000;
	for (int i=0; i<approx_chosen[maxAreaApproxInx].size(); i++){
		
		if(approx_chosen[maxAreaApproxInx][i].x > max_x){
			max_x = approx_chosen[maxAreaApproxInx][i].x;	
		}
		if(approx_chosen[maxAreaApproxInx][i].y > max_y){
			max_y = approx_chosen[maxAreaApproxInx][i].y;	
		}
		if(approx_chosen[maxAreaApproxInx][i].x < min_x){
			min_x = approx_chosen[maxAreaApproxInx][i].x;	
		}
		if(approx_chosen[maxAreaApproxInx][i].y < min_y){
			min_y = approx_chosen[maxAreaApproxInx][i].y;	
		}
		
	}
	for (int i=0; i<approx_chosen[maxAreaApproxInx].size(); i++){
		
		if(approx_chosen[maxAreaApproxInx][i].x == max_x){
			if(approx_chosen[maxAreaApproxInx][i].y == max_y && p[2] != 1){
				src[2] = approx_chosen[maxAreaApproxInx][i];
				ap[i]=1;
				p[2]=1;
			}
			else if(approx_chosen[maxAreaApproxInx][(i+1)%4].x < (max_x - (double)0.5*(max_x-min_x)) /*&& p[1] != 1*/){
				src[1] = approx_chosen[maxAreaApproxInx][i];
				ap[i]=1;
				p[1]=1;	
			}
			else if(p[2] != 1){
				src[2] = approx_chosen[maxAreaApproxInx][i];
				ap[i]=1;
				p[2]=1;
			}
	
		}
		if(approx_chosen[maxAreaApproxInx][i].x == min_x){
			if(approx_chosen[maxAreaApproxInx][i].y == min_y && p[0] != 1){
				src[0] = approx_chosen[maxAreaApproxInx][i];
				ap[i]=1;
				p[0]=1;
			}
			else if(approx_chosen[maxAreaApproxInx][(i+1)%4].x > (min_x + (double)0.5*(max_x-min_x)) /*&& p[3] != 1*/){
				src[3] = approx_chosen[maxAreaApproxInx][i];
				ap[i]=1;
				p[3]=1;	
			}
			else if(p[0] != 1){
				src[0] = approx_chosen[maxAreaApproxInx][i];
				ap[i]=1;
				p[0]=1;
			}
		}


		if(approx_chosen[maxAreaApproxInx][i].y == max_y){
			if(approx_chosen[maxAreaApproxInx][i].x == max_x && p[2] != 1){
				src[2] = approx_chosen[maxAreaApproxInx][i];
				ap[i]=1;
				p[2]=1;
			}
			else if(approx_chosen[maxAreaApproxInx][(i+1)%4].y < (max_y - (double)0.5*(max_y-min_y)) /*&& p[2] != 1*/){
				src[2] = approx_chosen[maxAreaApproxInx][i];
				ap[i]=1;
				p[2]=1;	
			}
			else if(p[3] != 1){
				src[3] = approx_chosen[maxAreaApproxInx][i];
				ap[i]=1;
				p[3]=1;
			}
	
		}
		if(approx_chosen[maxAreaApproxInx][i].y == min_y){
			if(approx_chosen[maxAreaApproxInx][i].x == min_x && p[0] != 1){
				src[0] = approx_chosen[maxAreaApproxInx][i];
				ap[i]=1;
				p[0]=1;
			}
			else if(approx_chosen[maxAreaApproxInx][(i+1)%4].y > (min_y + (double)0.5*(max_y-min_y)) /*&& p[0] != 1*/){
				src[0] = approx_chosen[maxAreaApproxInx][i];
				ap[i]=1;
				p[0]=1;	
			}
			else if(p[1] != 1){
				src[1] = approx_chosen[maxAreaApproxInx][i];
				ap[i]=1;
				p[1]=1;
			}
	
		}

	}
	for (int i=3; i>=0; i--){
		if(p[i] == 0){
			for(int j=0; j<approx_chosen[maxAreaApproxInx].size(); j++){
				if(ap[i] == 0){
					src[i] = approx_chosen[maxAreaApproxInx][j];
					p[i] = 1;
					ap[j] = 1;
				}
			}
		}
		
	}
	cout<<approx_chosen[maxAreaApproxInx]<<endl;
	cout<<src[0]<<" "<<src[1]<<" "<<src[2]<<" "<<src[3]<<endl;
	
	double heightL = sqrt(pow((src[0].x - src[3].x),2)+pow((src[0].y - src[3].y),2));
	double heightR = sqrt(pow((src[1].x - src[2].x),2)+pow((src[1].y - src[2].y),2));
	//cout<<"Left height: "<<heightL<<endl;
	//cout<<"Right height: "<<heightR<<endl;

	double widthUp = sqrt(pow((src[0].x - src[1].x),2)+pow((src[0].y - src[1].y),2));
	double widthDown = sqrt(pow((src[3].x - src[2].x),2)+pow((src[3].y - src[2].y),2));
	
	if (heightL > heightR){
		if(widthUp > heightL){
			qrAngle = acos(heightL/widthUp);	
		}
		else{
			qrAngle = acos(widthUp/heightL);
		}
					
	}
	else{
		if(widthUp > heightR){
			qrAngle = acos(heightR/widthUp);	
		}
		else{
			qrAngle = acos(widthUp/heightR);
		}
	}
	
	cout<<"heightL/widthUp "<<widthUp/heightL<<endl;
	cout<<"heightR/widthUp "<<widthUp/heightR<<endl;

	cout<<"Angle of inlination: "<<(double)(qrAngle*180/3.14)<<endl;
	//cout<<max_x<<endl;
	//cout<<min_x<<endl;
	//cout<<max_y<<endl;
	//cout<<min_y<<endl;
	pic.copyTo(perspQr);
	resize(perspQr,perspQr,Size(300,300));
	perspT = getPerspectiveTransform(src,dst);
	warpPerspective(pic, perspQr, perspT, Size(300,300));
	imshow("Qr", perspQr);
	imshow("Orginal", pic);
	waitKey(0);
	





}





















































