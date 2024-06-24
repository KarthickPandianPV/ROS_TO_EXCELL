#include "ros/ros.h"
#include "geometry_msgs/Vector3.h"
#include "xlsxwriter.h"
#include <time.h>

ros::Subscriber magnetic_sub;
ros::Subscriber acceleration_sub;
ros::Subscriber velocity_sub;
ros::Subscriber position_sub;

geometry_msgs::Vector3 magnetic_field;
geometry_msgs::Vector3 linear_acceleration;
geometry_msgs::Vector3 velocity;
geometry_msgs::Vector3 position;

bool acc_data_available = false, mag_data_available = false, velocity_data_available = false,pos_data_available=false;

time_t rawtime;
struct tm *timeinfo;
char date_string[30];
char time_string[30];


void magnetometerCallBack(const geometry_msgs::Vector3& magnetic_field_msg);
void accelerometerCallBack(const geometry_msgs::Vector3& acceleration_msg);
void velocityCallBack(const geometry_msgs::Vector3& velocity_msg);
void positionCallBack(const geometry_msgs::Vector3& position_msg);

double ax,ay,az,mx,my,mz;
double vx,vy,vz,sx,sy,sz;

int main(int argc,char** argv) 
{
    int row=1;
    char type[10]=".xlsx";
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(date_string, sizeof(date_string), "%Y-%m-%d+", timeinfo);
    strftime(time_string, sizeof(time_string), "%H-%M-%S", timeinfo);
    strcat(date_string,time_string);
    strcat(date_string,type);
    const char *wb_name=  &date_string[0];
    lxw_workbook  *workbook  = workbook_new(wb_name);
    lxw_format *format_bold = workbook_add_format(workbook);
    lxw_format *format_txt_clr = workbook_add_format(workbook);

    lxw_worksheet *worksheet = workbook_add_worksheet(workbook,time_string);
    format_set_bold(format_bold);
    format_set_font_color(format_txt_clr,LXW_COLOR_BLUE);

    worksheet_write_string(worksheet, 0, 0, "Timestamp",format_bold);
    worksheet_write_string(worksheet, 0, 1, "mx", format_bold);
    worksheet_write_string(worksheet, 0, 2, "my", format_bold);
    worksheet_write_string(worksheet, 0, 3, "mz", format_bold);
    worksheet_write_string(worksheet, 0, 5, "ax", format_bold);
    worksheet_write_string(worksheet, 0, 6, "ay", format_bold);
    worksheet_write_string(worksheet, 0, 7, "az", format_bold);
    worksheet_write_string(worksheet, 0, 9, "vx", format_bold);
    worksheet_write_string(worksheet, 0, 10, "vy", format_bold);
    worksheet_write_string(worksheet, 0, 11, "vz", format_bold);
    worksheet_write_string(worksheet, 0, 13, "sx", format_bold);
    worksheet_write_string(worksheet, 0, 14, "sy", format_bold);
    worksheet_write_string(worksheet, 0, 15, "sz", format_bold);

    ros::init(argc, argv, "data_logger");
    ros::NodeHandle n;
    magnetic_sub=n.subscribe("magnetic_field", 1, magnetometerCallBack);
    acceleration_sub =n.subscribe("linear_acceleration", 1, accelerometerCallBack);
    velocity_sub =n.subscribe("angular_velocity", 1, velocityCallBack);
    position_sub =n.subscribe("magnetic_field", 1, positionCallBack);
    
    while (ros::ok())
       {
           if((acc_data_available && mag_data_available) && (velocity_data_available && pos_data_available))
           {
              time(&rawtime);
              timeinfo = localtime(&rawtime);
              strftime(time_string, sizeof(time_string), "%H-%M-%S", timeinfo);
              
              worksheet_write_string(worksheet, row, 0, time_string,format_txt_clr);
              worksheet_write_number(worksheet, row, 1, mx, NULL);
              worksheet_write_number(worksheet, row, 2, my, NULL);
              worksheet_write_number(worksheet, row, 3, mz, NULL);
              worksheet_write_number(worksheet, row, 5, ax, NULL);
              worksheet_write_number(worksheet, row, 6, ay, NULL);
              worksheet_write_number(worksheet, row, 7, az, NULL);
              worksheet_write_number(worksheet, row, 9, vx, NULL);
              worksheet_write_number(worksheet, row, 10, vy, NULL);
              worksheet_write_number(worksheet, row, 11, vz, NULL);
              worksheet_write_number(worksheet, row, 13, sx, format_txt_clr);
              worksheet_write_number(worksheet, row, 14, sy, format_txt_clr);
              worksheet_write_number(worksheet, row, 15, sz, format_txt_clr);
              row++;
              acc_data_available = false; velocity_data_available = false; mag_data_available = false,pos_data_available=false;
           }
              ros::spinOnce();
       }

    
    return workbook_close(workbook);
}

void velocityCallBack(const geometry_msgs::Vector3& velocity_msg)
{
    vx = velocity_msg.x;
    vy = velocity_msg.y;
    vz = velocity_msg.z;

    velocity_data_available = true;
}

void accelerometerCallBack(const geometry_msgs::Vector3& acceleration_msg)
{
    ax = acceleration_msg.x;
    ay = acceleration_msg.y;
    az = acceleration_msg.z;

    acc_data_available = true;
}

void positionCallBack(const geometry_msgs::Vector3& position_msg)
{
    sx = position_msg.x;
    sy = position_msg.y;
    sz = position_msg.z;
    
    pos_data_available = true;
}

void magnetometerCallBack(const geometry_msgs::Vector3& magnetic_field_msg)
{
    mx = magnetic_field_msg.x;
    my = magnetic_field_msg.y;
    mz = magnetic_field_msg.z;

    mag_data_available = true;
}


