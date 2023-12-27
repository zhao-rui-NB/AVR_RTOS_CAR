//will follow black line
// #define ADC_L_BLACK 28
// #define ADC_M_BLACK 43
// #define ADC_R_BLACK 28
// #define ADC_L_WHITE 543
// #define ADC_M_WHITE 487
// #define ADC_R_WHITE 533

#define ADC_L_BLACK 43
#define ADC_M_BLACK 61
#define ADC_R_BLACK 35
#define ADC_L_WHITE 488
#define ADC_M_WHITE 379
#define ADC_R_WHITE 487

// if < threshold, then black
#define ADC_L_THRESHOLD  ((ADC_L_BLACK + ADC_L_WHITE)/2)
#define ADC_M_THRESHOLD  ((ADC_M_BLACK + ADC_M_WHITE)/2)
#define ADC_R_THRESHOLD  ((ADC_R_BLACK + ADC_R_WHITE)/2)

#define TARGRT_SPEED 75 //75ok

// test v3 value 
#define TEST_ADC_L_BLACK 43
#define TEST_ADC_M_BLACK 61
#define TEST_ADC_R_BLACK 35
#define TEST_ADC_L_WHITE 488
#define TEST_ADC_M_WHITE 379
#define TEST_ADC_R_WHITE 487



// v1 garbage 
/*
void task_pid_line_follow(void *arg){
    while(1){
        uint8_t sensor_encode;
        int error;

        sensor_encode = (adc_L_get_avg() < ADC_L_THRESHOLD) << 2 | 
                        (adc_M_get_avg() < ADC_M_THRESHOLD) << 1 | 
                        (adc_R_get_avg() < ADC_R_THRESHOLD) << 0;

        switch(sensor_encode){
            case 0b100:
                error = -2;
                break;
            case 0b001:
                error = 2;
                break;
            case 0b110:
                error = -1;
                break;
            case 0b011:
                error = 1;
                break;
            default:
                error = 0;
                break;            
        }

        // https://www.eet-china.com/mp/a119142.html?fbclid=IwAR26cIluPrgb1PNj0koLSO22H0IaeI4kiDMwyXBFAb6d3fO0tYFLTYMmFaQ 
        int Kp = 50;
        int Kd = 5;
        int Ki = 1;
        static int prev_error = 0;
        static int prev_prev_error = 0;
        static int prev_u = 0;
        uint16_t dt = 1;

        int p = error-prev_error;
        int i = error;
        int d = error-2*prev_error+prev_prev_error;

        int delta_u = Kp*p + Ki*i + Kd*d;
        int u = prev_u + delta_u;



        prev_prev_error = prev_error;
        prev_error = error;
        prev_u = u;

        int left_speed = TARGRT_SPEED + u;
        int right_speed = TARGRT_SPEED - u;


        if(left_speed >=0 && right_speed >=0){
            motor_write_dir(MOTOR_DIR_FF);
        }else if(left_speed < 0 && right_speed >= 0){
            motor_write_dir(MOTOR_DIR_BF);
            left_speed = left_speed*-1;
        }else if(left_speed >= 0 && right_speed < 0){
            motor_write_dir(MOTOR_DIR_FB);
            right_speed = right_speed*-1;
        }else{
            motor_write_dir(MOTOR_DIR_BB);
            left_speed = left_speed*-1;
            right_speed = right_speed*-1;
        }        
        if(left_speed > 1023) left_speed = 1023;
        if(right_speed > 1023) right_speed = 1023;


        motor_write_speed(left_speed, right_speed);
        serial_print("L , R : ");
        serial_print(left_speed);
        serial_print(" , ");
        serial_print(right_speed);

        //sensor_encode
        serial_print("  sensor_encode : ");
        serial_print(sensor_encode);
        serial_print("\n");
        sleep(5);
    }

}
*/
// V2 
void task_pid_line_follow(void *arg){
    while(1){
        uint8_t sensor_encode;
        static float integral = 0;
        int error;
    
        static uint8_t last_sensor_encode = 0b010;

        sensor_encode = (adc_L_get_avg() < ADC_L_THRESHOLD) << 2 | 
                        (adc_M_get_avg() < ADC_M_THRESHOLD) << 1 | 
                        (adc_R_get_avg() < ADC_R_THRESHOLD) << 0;

        write_led2((adc_L_get_avg() < ADC_L_THRESHOLD));
        write_led3((adc_M_get_avg() < ADC_M_THRESHOLD));
        write_led4((adc_R_get_avg() < ADC_R_THRESHOLD));

        // switch(sensor_encode){
        //     case 0b010:
        //         error = 0;
        //         break;
        //     case 0b100:
        //         error = -2;
        //         break;
        //     case 0b001:
        //         error = 2;
        //         break;
        //     case 0b110:
        //         error = -1;
        //         break;
        //     case 0b011:
        //         error = 1;
        //         break;
        //     case 0b000:
        //         if(last_sensor_encode == 0b100){
        //             error = -3;
        //         }
        //         else if(last_sensor_encode == 0b001){
        //             error = 3;
        //         }
        //         break;
        //     default:
        //         error = 0;
        //         break;            
        // }

        switch(sensor_encode){
            case 0b010:
                error = 0;
                integral = 0;
                break;
            case 0b100:
                error = -1;
                break;
            case 0b001:
                error = 1;
                break;
            case 0b110:
                error = -1;
                break;
            case 0b011:
                error = 1;
                break;
            case 0b000:
                if(last_sensor_encode == 0b100 || last_sensor_encode == 0b110){
                    error = -3;
                }
                else if(last_sensor_encode == 0b001 || last_sensor_encode == 0b011){
                    error = 3;
                }
            default:
                error = 0;
        }

        last_sensor_encode = sensor_encode;

        // https://www.eet-china.com/mp/a119142.html?fbclid=IwAR26cIluPrgb1PNj0koLSO22H0IaeI4kiDMwyXBFAb6d3fO0tYFLTYMmFaQ 
        float Kp = 65;
        float Kd = 10;
        float Ki = 0;
        // int i
        float dt = 3;

        
        static float prev_error = 0;
        static float derivative = 0;  

        integral += error*dt;
        derivative = (error-prev_error)/dt;

        float p = error;
        float i = integral;
        float d = derivative;

            

        int out = Kp*p + Ki*i + Kd*d;


        int left_speed = TARGRT_SPEED + out;
        int right_speed = TARGRT_SPEED - out;
        
        serial_print("out : ");
        serial_print(out);

        serial_print(" L , R : ");
        serial_print(left_speed);
        serial_print(" , ");
        serial_print(right_speed);

        if(left_speed >=0 && right_speed >=0){
            serial_print(" *FF* ");
            motor_write_dir(MOTOR_DIR_FF);
        }else if(left_speed < 0 && right_speed >= 0){
            serial_print(" *BF* ");
            motor_write_dir(MOTOR_DIR_BF);
            left_speed = left_speed*-1;
        }else if(left_speed >= 0 && right_speed < 0){
            serial_print(" *FB* ");
            motor_write_dir(MOTOR_DIR_FB);
            right_speed = right_speed*-1;
        }else{
            serial_print(" *BB* ");
            motor_write_dir(MOTOR_DIR_BB);
            left_speed = left_speed*-1;
            right_speed = right_speed*-1;
        }        
        if(left_speed > 1023) left_speed = 1023;
        if(right_speed > 1023) right_speed = 1023;

        serial_print(" L , R : ");
        serial_print(left_speed);
        serial_print(" , ");
        serial_print(right_speed);

        motor_write_speed(left_speed, right_speed);


        serial_print("\n");
        sleep(dt);
    }

}

// V3 use analog sensor input
void task_pid_line_follow_V3(void *arg){
    while(1){
        static float integral = 0;
        static float last_error = 0;
        float error;

        float L = (float)adc_L_get_avg();
        float M = (float)adc_M_get_avg();
        float R = (float)adc_R_get_avg();

        // map adc to 0-1 
        float L_map = float(L-TEST_ADC_L_BLACK)/float(TEST_ADC_L_WHITE-TEST_ADC_L_BLACK);
        //float M_map = (M-TEST_ADC_M_BLACK)/(TEST_ADC_M_WHITE-TEST_ADC_M_BLACK);
        float R_map = float(R-TEST_ADC_R_BLACK)/float(TEST_ADC_R_WHITE-TEST_ADC_R_BLACK);
        int pos = (L_map-R_map)*1000.0;

        error = (L_map-R_map);
        if(L_map>0.95 && R_map>0.95){
            if(last_error > 0){
                error = 1.0;
            }else{
                error = -1.0;
            }
        }

        last_error = error;

        // https://www.eet-china.com/mp/a119142.html?fbclid=IwAR26cIluPrgb1PNj0koLSO22H0IaeI4kiDMwyXBFAb6d3fO0tYFLTYMmFaQ 
        float Kp = 57;
        float Kd = 5;
        float Ki = 0;
        // int i
        float dt = 3;

        
        static float prev_error = 0;
        static float derivative = 0;  

        integral += error*dt;
        derivative = (error-last_error)/dt;

        float p = error;
        float i = integral;
        float d = derivative;

            

        int out = Kp*p + Ki*i + Kd*d;


        int left_speed = TARGRT_SPEED + out;
        int right_speed = TARGRT_SPEED - out;
        
        serial_print("out : ");
        serial_print(out);

        serial_print(" L , R : ");
        serial_print(left_speed);
        serial_print(" , ");
        serial_print(right_speed);

        if(left_speed >=0 && right_speed >=0){
            serial_print(" *FF* ");
            motor_write_dir(MOTOR_DIR_FF);
        }else if(left_speed < 0 && right_speed >= 0){
            serial_print(" *BF* ");
            motor_write_dir(MOTOR_DIR_BF);
            left_speed = left_speed*-1;
        }else if(left_speed >= 0 && right_speed < 0){
            serial_print(" *FB* ");
            motor_write_dir(MOTOR_DIR_FB);
            right_speed = right_speed*-1;
        }else{
            serial_print(" *BB* ");
            motor_write_dir(MOTOR_DIR_BB);
            left_speed = left_speed*-1;
            right_speed = right_speed*-1;
        }        
        if(left_speed > 1023) left_speed = 1023;
        if(right_speed > 1023) right_speed = 1023;

        serial_print(" L , R : ");
        serial_print(left_speed);
        serial_print(" , ");
        serial_print(right_speed);

        motor_write_speed(left_speed, right_speed);


        serial_print("\n");
        sleep(dt);
    }

}



// V4 use analog sensor input
void task_pid_line_follow_V4(void *arg){
    while(1){
        static float integral = 0;
        static float last_error = 0;
        float error;
        uint8_t sensor_encode;
        
        // 偏差累加值 累積 
        const float pos_side_error_unit_scale = 0.05;

        sensor_encode = (adc_L_get_avg() < ADC_L_THRESHOLD) << 2 | 
                        (adc_M_get_avg() < ADC_M_THRESHOLD) << 1 | 
                        (adc_R_get_avg() < ADC_R_THRESHOLD) << 0;

        static float pos_side_error = 0;


        if(sensor_encode & 0b100){
            pos_side_error = 0;
            error = -3;
        }
        else if(sensor_encode & 0b010){
            pos_side_error = 0;
            error = -2;
        }
        else if(sensor_encode & 0b001){
            pos_side_error = 0;
            error = -1;
        }
        else{

            pos_side_error += pos_side_error_unit_scale;
            error = pos_side_error;
        }
       
       // print error 
        serial_print("error : ");
        serial_print( (int)(error*1000.0));


        last_error = error;

        // https://www.eet-china.com/mp/a119142.html?fbclid=IwAR26cIluPrgb1PNj0koLSO22H0IaeI4kiDMwyXBFAb6d3fO0tYFLTYMmFaQ 
        float Kp = 80;
        float Kd = 0;
        float Ki = 0;
        // int i
        float dt = 3;

        
        // static float prev_error = 0;
        static float derivative = 0;  

        integral += error*dt;
        derivative = (error-last_error)/dt;

        float p = error;
        float i = integral;
        float d = derivative;

            
        int out;
        // out = (float)Kp*p + Ki*i + Kd*d;
        out = Kp*p;
        serial_print("kp:"  );
        serial_print((int)Kp);
        serial_print("p:"  );
        serial_print((int)p);
        serial_print("kp*p:"  );
        serial_print(int(Kp*p));
        serial_print("out : ");
        serial_print(out);


        int left_speed = TARGRT_SPEED + out;
        int right_speed = TARGRT_SPEED - out;
        

        if(left_speed >=0 && right_speed >=0){
            motor_write_dir(MOTOR_DIR_FF);
        }else if(left_speed < 0 && right_speed >= 0){
            motor_write_dir(MOTOR_DIR_BF);
            left_speed = left_speed*-1;
        }else if(left_speed >= 0 && right_speed < 0){
            motor_write_dir(MOTOR_DIR_FB);
            right_speed = right_speed*-1;
        }else{
            motor_write_dir(MOTOR_DIR_BB);
            left_speed = left_speed*-1;
            right_speed = right_speed*-1;
        }        
        if(left_speed > 1023) left_speed = 1023;
        if(right_speed > 1023) right_speed = 1023;

        serial_print(" L , R : ");
        serial_print(left_speed);
        serial_print(" , ");
        serial_print(right_speed);

        motor_write_speed(left_speed, right_speed);


        serial_print("\n");
        sleep(dt);
    }

}



void sensor_map(void *arg){
    while(1){
        float L = (float)adc_L_get_avg();
        float M = (float)adc_M_get_avg();
        float R = (float)adc_R_get_avg();

        // map adc to 0-1 
        float L_map = float(L-TEST_ADC_L_BLACK)/float(TEST_ADC_L_WHITE-TEST_ADC_L_BLACK);
        //float M_map = (M-TEST_ADC_M_BLACK)/(TEST_ADC_M_WHITE-TEST_ADC_M_BLACK);
        float R_map = float(R-TEST_ADC_R_BLACK)/float(TEST_ADC_R_WHITE-TEST_ADC_R_BLACK);
        
        int pos = (R_map-L_map)*1000.0;

        serial_print("L map , R map : ");
        serial_print(int(L_map*1000.0));
        serial_print(" , ");
        serial_print(int(R_map*1000.0));

        serial_print("\tpos : ");
        serial_print(pos);


        serial_print("\n");


        // serial_print("L , M , R : ");
        // serial_print(L);
        // serial_print(" , ");
        // serial_print(M);
        // serial_print(" , ");
        // serial_print(R);
        // serial_print("pos : ");
        // serial_print(pos);
        // serial_print("\n");

        sleep(200);
    }
}

