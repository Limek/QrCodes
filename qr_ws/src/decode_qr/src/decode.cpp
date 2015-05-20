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

using namespace std;
using namespace cv;
using namespace ros;
using namespace zbar;

int main(int argc, char **argv){

init(argc, argv, "decode");
NodeHandle n;
Publisher chatter_pub = n.advertise<std_msgs::String>("chatter", 1000);
ros::Rate loop_rate(10); // wysyłanie z częstotoliwością 10 Hz

while (ros::ok())
{
	//Mat pic = imread("qr.png",0);
	//Mat pic = imread("IMG_0001.JPG",0);
	Mat pic = imread("qr5.jpg",0);
	if( !pic.data ){
		int k = 0;
		cout<<"Brak foty"<<endl;
	     	return -1;
	}

	string msg_decoded;
	code_qr qr_obj;
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
