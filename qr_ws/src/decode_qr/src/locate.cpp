#include <ros/ros.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv/cv.h"
#include <stdio.h>
#include <iostream>
#include "zbar.h"
#include "decode_qr_code.h"
#include "std_msgs/String.h"
#include <sstream>
#include <math.h>
#include <fstream>

using namespace std;
using namespace cv;
using namespace ros;
using namespace zbar;

static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0){
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

ifstream file; //handle for file 
fstream file_out("wyniki/Limanski_Michal.txt", ios::out); //handle for file with results
string pic_name;

int main(int argc, char **argv){

init(argc, argv, "locate");
NodeHandle n;
Publisher chatter_pub = n.advertise<std_msgs::String>("locator", 1000);
ros::Rate loop_rate(10); // wysyłanie z częstotoliwością 10 Hz

	file.open("dane_wejsciowe/nazwy_zdjec.txt");
	if (file.good()){
		cout << "File has been opened succesfully!" << endl;
	}


while (ros::ok() && !file.eof())
{
	Mat pic;

	getline(file, pic_name);
cout<<pic_name<<endl;	
		pic = imread(pic_name,0);
		if (pic.empty()){
		
			file_out << " " << endl;
			cout << "nie wczytano zdjecia" << endl;
			continue;
		}
	/*
	//Mat pic = imread("qr.png",0);
	//Mat pic = imread("IMG_0001.JPG",0);
	Mat pic = imread("qr5.jpg",0);
	//Mat pic = imread("masterpiece.png",0);*/
	if( !pic.data ){
		int k = 0;
		cout<<"Brak foty"<<endl;
	     	return -1;
	}
	
	


	

	string msg_decoded;
	code_qr qr_obj;
	
	// Okreslenie polozenia na zdjeciu - od tego miejsca jest kod ktory docelowo mialby byc
	// w metodzie naszej klasy ale dla testow zrobilem go tutaj
	
	// deklaracja zmiennych z ktorych bedziemy korzystali
	Mat canny_out;
	Mat canny_out2;
	Mat contoursPic;

	vector<vector<Point> > contours;
	
	vector<vector<Point> > selected_contours;
	vector<vector<Point> > selected_contoursR3;
	vector<Vec4i> hierarchy;
	
	//resize(pic,pic,Size,768,1024));
	// zmieniam rozmiar zeby byl jakos normalny gdy chce wyswietlic do testu
	// docelowo zbedne, duzy obrazek w niczym nie przeszkadza 
	resize(pic,pic,Size(480,600));
	pic.copyTo(qr_obj.pic);
	Canny(pic,canny_out,100,200);
	pic.copyTo(contoursPic);
	findContours(canny_out,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE); // szukam konturow na calym obrazku
	//drawContours(contoursPic, contours, -1, Scalar(255,0,0), 3, 8);
	// petla w ktorej analizuje kazdy kontur
	
///// Z NETA
	for(int i=0; i<contours.size(); i++){
		qr_obj.add_contour(contours[i]);

	}
	//qr_code.add_contour();
	//qr_obj.detect_squares();
	qr_obj.detect_the_chosen_one();
	//if(!(qr_obj.squares_contours.empty())){
	//	drawContours(contoursPic, qr_obj.squares_contours, -1, Scalar(255,0,0), 2, 8);
	//	imshow("Pic2",contoursPic);
	//	waitKey(0);

	//}
	/*vector<Point> approx;

	for (size_t i = 0; i < contours.size(); i++)
	{
	    	approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

	    	if (approx.size() == 4 &&
			fabs(contourArea(Mat(approx))) > 1000 &&
			isContourConvex(Mat(approx)))
	    	{
		double maxCosine = 0;

		for( int j = 2; j < 5; j++ )
		{
		    	double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
		    	maxCosine = MAX(maxCosine, cosine);
		}

		if( maxCosine < 0.3 )
		    	selected_contours.push_back(approx);
	    }
	}
	if(!(selected_contours.empty())){
		drawContours(contoursPic, selected_contours, -1, Scalar(255,0,0), 2, 8);
		imshow("Pic2",contoursPic);
		waitKey(0);

	}*/






//// MOJA KONCEPCJA
	/*for(int i=0; i<contours.size(); i++){
		RotatedRect rRect=minAreaRect(contours[i]); // rysuje minimalny prostokat dookola konturu
		Size rRect_size = rRect.size;
		// sprawdzam czy wymiary prostokata zblizone sa do kwadratu, przyjete wspolczynniki 0.8 i 1.2 sa w miare optymalne ale to mozna jeszcze udoskonalic
		// w kazdym razie wynajduje wszystkie kontury obrysowane kwadratami
		if(((double)rRect_size.width/(double)rRect_size.height)>0.80 && ((double)rRect_size.width/(double)rRect_size.height)<1.20){
			selected_contours.push_back(contours[i]); // kontur spelniajacy kryterium pushuje na wektor wyselekcjonowanych konturow
		}
	}
	// tutaj moja wesola tworczosc - analizuje te wybrane kontury... 
	for(int i=0; i<selected_contours.size(); i++){

		vector<vector<Point> > contours2;
		vector<Vec4i> hierarchy2;
		RotatedRect rect = minAreaRect(selected_contours[i]);// ... znowu rysuje prostokat wokol konturu
		//Point2f vertices[4];
			//rect.points(vertices);
				//for (int j = 0; j < 4; j++)
    					//line(contoursPic, vertices[j], vertices[(j+1)%4], Scalar(0,0,0));
		// wycinam ten prostokat
		Mat M, rotated, cropped;
        	float angle = rect.angle;
        	Size rect_size = rect.size;
        	if (rect.angle < -45.) {
           		angle += 90.0;
            		swap(rect_size.width, rect_size.height);
       	 	}
        	M = getRotationMatrix2D(rect.center, angle, 1.0);
        	warpAffine(pic, rotated, M, pic.size(), INTER_CUBIC);
        	getRectSubPix(rotated, rect_size, rect.center, cropped);
		// koniec wycinania - dostaje mniejszy obrazek

		//imshow("wyciete",cropped);
		//waitKey(0);
		Size crop_size = cropped.size();
		cout<<"Wys cropped: "<<crop_size.height<<endl;
		cout<<"Szer cropped: "<<crop_size.width<<endl;
		Canny(cropped,canny_out2,100,200);
		findContours(canny_out2,contours2,hierarchy2,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE); // na tym wycietym znowu szukam konturow
		int cnt=0;
		for(int j=0; j<contours2.size(); j++){
			
			RotatedRect r = minAreaRect(contours2[j]); // i znowu obrysowuje te znalezione kontury na tym mniejszym obrazku 
			Size r_size = r.size;
			// i ponizej chcialem wyszukac jakies kryterium sensowne ktory mi rozrozni te wyciete male obrazki na ktorych sa te znaczniki
			// wiemy ze kazdy z tych 3 znacznikow to maly czarny kwadracik obrysowany wieksza ramka
			// tak wiec jesli wycialem maly obrazek ktory obejmuje caly znacznik to wewnatrz tego obrazka powinny byc max 2 kontury - tez kwadratowe
			// i takie kryterium chcialem przyjac ! ...
			// ... ale nie dziala :(
			//if((double)r_size.width<(double)0.5*(double)crop_size.width && (double)r_size.height<(double)0.5*(double)crop_size.height && ((double)r_size.height/(double)r_size.width)>0.8 && ((double)r_size.height/(double)r_size.width)<1.2 ){
							
						
			if(((double)r_size.height/(double)r_size.width)>0.8 && ((double)r_size.height/(double)r_size.width)<1.2 ){
				if((double)r_size.height > 0.4*(double)crop_size.height && (double)r_size.width > 0.4*(double)crop_size.width){
					cout<<"Rama konturu wys: "<<r_size.height<<endl;
					cout<<"Rama konturu szer: "<<r_size.width<<endl;
					Point2f vertices[4];
					r.points(vertices);
					for (int k = 0; k < 4; k++)
    						line(cropped, vertices[k], vertices[(k+1)%4], Scalar(0,0,0));
					cnt++;
					drawContours(cropped, contours2, j+1, Scalar(255,0,0), 2, 8);
					imshow("Pic",cropped);
					waitKey(0);
				}
								
				//drawContours(cropped, contours2, -1, Scalar(255,0,0), 2, 8);
				//imshow("Pic",cropped);
				//waitKey(0);
				//selected_contoursR3.push_back(selected_contours[i]);
			} 




		}
		//if(contours2.size()<2){
			//selected_contoursR3.push_back(selected_contours[i]);
			
		//}
		if(cnt<3){
			//drawContours(cropped, contours2, -1, Scalar(255,0,0), 2, 8);
			//imshow("Pic",cropped);
			//waitKey(0);
			selected_contoursR3.push_back(selected_contours[i]);
			
		}
	}
	// tutaj sobie wysuje te kontury ktore powinny obrysowywac znaczniki ale niestety chwyta jakies smieci 
	// poki co nie mam lepszego pomyslu
	// najlepiej by bylo ogarnac z ramka wtedy wystarczy kazdy kwadrat sprawdzic czy cos czyta i bedzie viola cycyus glancus
	// ale to mozna napisac hop siup, a my pewnie chcemy byc pro i nie korzystac z ramki :D
	// a ze jestesmy zajebisci to pewnie nam sie uda bez ramki :D
	if(!(selected_contoursR3.empty())){
		drawContours(contoursPic, selected_contoursR3, -1, Scalar(255,0,0), 2, 8);
		imshow("Pic2",contoursPic);
		waitKey(0);

	}*/
	//drawContours(contoursPic, selected_contours, -1, Scalar(255,0,0), 3, 8);
	//cout<<selected_contours[0]<<endl;
	//imshow("Pic",contoursPic);
	//waitKey(0);





	// ponizej ROSowe pierdoly zeby publikowac odczytane dane w jakims temacie 
	// (zeby bylo pro) :D
	qr_obj.decoder(pic, msg_decoded);
	cout<<"Odczytana wiadomosc: "<<msg_decoded<<endl;
	//waitKey(0);

    	std_msgs::String msg;
    	std::stringstream ss;
    	ss << msg_decoded;
    	msg.data = ss.str();

    	//ROS_INFO("%s", msg.data.c_str());
    	chatter_pub.publish(msg);

    	ros::spinOnce();

    	loop_rate.sleep();
}

return 0;

}
