#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>
//#include <cont_ad.h>
#include <sphinxbase/cmd_ln.h>
#include <pocketsphinx/pocketsphinx.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "Aria.h"
#include <queue>
#define MODELDIR "/usr/local/share/pocketsphinx/model"


using namespace std;
static ps_decoder_t *ps;
static cmd_ln_t *config;


ArRobot robot1;
ArSonarDevice sonar;

std::queue <std::string> myQue;
static void sleep_msec(int32 ms)
{
    struct timeval tmo;

    tmo.tv_sec = 0;
    tmo.tv_usec = ms * 1000;

    select(0, NULL, NULL, NULL, &tmo);
}

static void *recognize_from_microphone(void *dummy)
{
    ad_rec_t *ad;
    int16 adbuf[2048];
    uint8 utt_started, in_speech;
    int32 k;
    char const *hyp;
    //string s;

    if ((ad = ad_open_dev(cmd_ln_str_r(config, ""),
                          (int) cmd_ln_float32_r(config,
                                  "-samprate"))) == NULL)
        E_FATAL("Failed to open audio device\n");
    if (ad_start_rec(ad) < 0)
        E_FATAL("Failed to start recording\n");

    if (ps_start_utt(ps) < 0)
        E_FATAL("Failed to start utterance\n");
    utt_started = FALSE;
    E_INFO("~ Ready to listen ~\n");

    for (;;)
    {
        if ((k = ad_read(ad, adbuf, 2048)) < 0)
            E_FATAL("Failed to read audio\n");
        ps_process_raw(ps, adbuf, k, FALSE, FALSE);
        in_speech = ps_get_in_speech(ps);
        if (in_speech && !utt_started)
        {
            utt_started = TRUE;
            E_INFO("~ Listening ~\n");
        }
        if (!in_speech && utt_started)
        {
            ps_end_utt(ps);
            hyp = ps_get_hyp(ps, NULL );
            if (hyp != NULL)
            {
                char *a = const_cast<char*>(hyp);
                cout <<"======"<< a << "======"<< endl;

                char *pT;
                const char *d = " ";

                //strtok breaks the inputted variable "a" into tokens by the delinieator "d", which is a space in this case
                pT = strtok(a,d);
                while(pT)
                {
                    //puts the tokens into the global variable myQue...
                    myQue.push(pT);
                    //printf("%s\n",myQue);
                    //...and then goes onto the next one
                    pT = strtok(NULL,d);
                }

                //print the queue out for testing purposes
                cout << "~~ current queue ~~" << endl;
                std::queue<std::string> q = myQue;
                while(!q.empty()){
                  std::cout << q.front() << ' ';
                  q.pop();
                }

                fflush(stdout);
            }

            if (ps_start_utt(ps) < 0)
                E_FATAL("Failed to start utterance\n");
            utt_started = FALSE;
            E_INFO("~ Ready to listen ~\n");
        }
        sleep_msec(100);
    }
    ad_close(ad);
}

static void *Robot(void *dummy)
{
    std::string name;
    std::string command;
    std::string precommand;

    while (1)
    {
        //input = getchar();
        if(myQue.size() == 2)
        {
            cout<<"=========="<<"name"<<"=========="<<endl;
            cout<<myQue.front()<<endl;
            name = myQue.front();
            if(name=="mary")
            {
                robot1.lock();
                robot1.clearDirectMotion();
                robot1.enableMotors();
                robot1.unlock();
                myQue.pop();
                command = myQue.front();
                cout << "========command=========" << endl;
                cout<<myQue.front()<<endl;
                if(command=="forward")
                {
                    robot1.setRotVel(0);
                    robot1.setVel(100);
                    ArUtil::sleep(5000);
                    robot1.setVel(0);
                }
                else if(command=="backward")
                {
                    robot1.setRotVel(0);
                    robot1.setVel(-100);
                    ArUtil::sleep(5000);
                    robot1.setVel(0);
                }
                else if(command=="left")
                {
                    robot1.setRotVel(30);
                    robot1.setVel(0);
                    ArUtil::sleep(3000);
                    robot1.setRotVel(0);
                    robot1.setVel(100);
                    ArUtil::sleep(5000);
                    robot1.setVel(0);
                }
                else if(command=="right")
                {
                    robot1.setRotVel(-30);
                    robot1.setVel(0);
                    ArUtil::sleep(3000);
                    robot1.setRotVel(0);
                    robot1.setVel(100);
                    ArUtil::sleep(5000);
                    robot1.setVel(0);
                }
                else if(command== "faster")
                {
                    if(precommand == "forward"||precommand == "left"||precommand == "right")
                    {
                        robot1.setRotVel(0);
                        robot1.setVel(200);
                        ArUtil::sleep(5000);
                        robot1.setVel(0);
                    }else if(precommand == "backward")
                    {
                        robot1.setRotVel(0);
                        robot1.setVel(-200);
                        ArUtil::sleep(5000);
                        robot1.setVel(0);
                    }
                }
                else if(command == "slower")
                {
                    if(precommand == "forward"||precommand == "left"||precommand == "right")
                    {
                        robot1.setRotVel(0);
                        robot1.setVel(50);
                        ArUtil::sleep(5000);
                        robot1.setVel(0);
                    }else if(precommand == "backward")
                    {
                        robot1.setRotVel(0);
                        robot1.setVel(-50);
                        ArUtil::sleep(5000);
                        robot1.setVel(0);
                    }
                }
                else if(command=="stop")
                {
                    robot1.setRotVel(0);
                    robot1.setVel(0);
                }
                precommand = myQue.front();
                myQue.pop();
            }
            // else if(name=="piggy")
            // {
            //    robot2.lock();
            //    robot2.clearDirectMotion();
            //    robot2.enableMotors();
            //    robot2.unlock();
            //     myQue.pop();
            //     command = myQue.front();
            //     //cout<<myQue.front()<<endl;
            //     if(command=="forward")
            //     {
            //        robot2.setRotVel(0);
            //        robot2.setVel(100);
            //         ArUtil::sleep(5000);
            //        robot2.setVel(0);
            //     }
            //     else if(command=="backward")
            //     {
            //        robot2.setRotVel(0);
            //        robot2.setVel(-100);
            //         ArUtil::sleep(5000);
            //        robot2.setVel(0);
            //     }
            //     else if(command=="left")
            //     {
            //        robot2.setRotVel(30);
            //        robot2.setVel(0);
            //         ArUtil::sleep(3000);
            //        robot2.setRotVel(0);
            //        robot2.setVel(100);
            //         ArUtil::sleep(5000);
            //        robot2.setVel(0);
            //     }
            //     else if(command=="right")
            //     {
            //        robot2.setRotVel(-30);
            //        robot2.setVel(0);
            //         ArUtil::sleep(3000);
            //        robot2.setRotVel(0);
            //        robot2.setVel(100);
            //         ArUtil::sleep(5000);
            //        robot2.setVel(0);
            //     }
            //     else if(command== "faster")
            //     {
            //         if(precommand == "forward"||precommand == "left"||precommand == "right")
            //         {
            //            robot2.setRotVel(0);
            //            robot2.setVel(200);
            //             ArUtil::sleep(5000);
            //            robot2.setVel(0);
            //         }else if(precommand == "backward")
            //         {
            //            robot2.setRotVel(0);
            //            robot2.setVel(-200);
            //             ArUtil::sleep(5000);
            //            robot2.setVel(0);
            //         }
            //     }
            //     else if(command == "slower")
            //     {
            //         if(precommand == "forward"||precommand == "left"||precommand == "right")
            //         {
            //            robot2.setRotVel(0);
            //            robot2.setVel(50);
            //             ArUtil::sleep(5000);
            //            robot2.setVel(0);
            //         }else if(precommand == "backward")
            //         {
            //             robot2.setRotVel(0);
            //             robot2.setVel(-50);
            //             ArUtil::sleep(5000);
            //             robot2.setVel(0);
            //         }
            //     }
            //     else if(command=="stop")
            //     {
            //         robot2.setRotVel(0);
            //         robot2.setVel(0);
            //     }
            //     precommand = myQue.front();
            //     myQue.pop();
            // }
            // else if(name=="both")
            // {
            //     robot1.lock();
            //     robot1.clearDirectMotion();
            //     robot1.enableMotors();
            //     robot1.unlock();
            //
            //     robot2.lock();
            //     robot2.clearDirectMotion();
            //     robot2.enableMotors();
            //     robot2.unlock();
            //     myQue.pop();
            //     command = myQue.front();
            //     //cout<<myQue.front()<<endl;
            //     if(command=="forward")
            //     {
            //         robot1.setRotVel(0);
            //         robot1.setVel(100);
            //         robot2.setRotVel(0);
            //         robot2.setVel(100);
            //         ArUtil::sleep(5000);
            //         robot1.setVel(0);
            //         robot2.setVel(0);
            //     }
            //     else if(command=="backward")
            //     {
            //         robot1.setRotVel(0);
            //         robot1.setVel(-100);
            //         robot2.setRotVel(0);
            //         robot2.setVel(-100);
            //         ArUtil::sleep(5000);
            //         robot1.setVel(0);
            //         robot2.setVel(0);
            //     }
            //     else if(command=="left")
            //     {
            //         robot1.setRotVel(30);
            //         robot1.setVel(0);
            //         robot2.setRotVel(30);
            //         robot2.setVel(0);
            //         ArUtil::sleep(3000);
            //         robot1.setRotVel(0);
            //         robot1.setVel(100);
            //         robot2.setRotVel(0);
            //         robot2.setVel(100);
            //         ArUtil::sleep(5000);
            //         robot1.setVel(0);
            //         robot2.setVel(0);
            //     }
            //     else if(command=="right")
            //     {
            //         robot1.setRotVel(-30);
            //         robot1.setVel(0);
            //         robot2.setRotVel(-30);
            //         robot2.setVel(0);
            //         ArUtil::sleep(3000);
            //         robot1.setRotVel(0);
            //         robot1.setVel(100);
            //         robot2.setRotVel(0);
            //         robot2.setVel(100);
            //         ArUtil::sleep(5000);
            //         robot1.setVel(0);
            //         robot2.setVel(0);
            //     }
            //     else if(command== "faster")
            //     {
            //         if(precommand == "forward"||precommand == "left"||precommand == "right")
            //         {
            //             robot1.setRotVel(0);
            //             robot1.setVel(200);
            //             robot2.setRotVel(0);
            //             robot2.setVel(200);
            //             ArUtil::sleep(5000);
            //             robot1.setVel(0);
            //             robot2.setVel(0);
            //         }else if(precommand == "backward")
            //         {
            //             robot1.setRotVel(0);
            //             robot1.setVel(-200);
            //             robot2.setRotVel(0);
            //             robot2.setVel(-200);
            //             ArUtil::sleep(5000);
            //             robot1.setVel(0);
            //             robot2.setVel(0);
            //         }
            //     }
            //     else if(command == "slower")
            //     {
            //         if(precommand == "forward"||precommand == "left"||precommand == "right")
            //         {
            //             robot1.setRotVel(0);
            //             robot1.setVel(50);
            //             robot2.setRotVel(0);
            //             robot2.setVel(50);
            //             ArUtil::sleep(5000);
            //             robot1.setVel(0);
            //             robot2.setVel(0);
            //         }else if(precommand == "backward")
            //         {
            //             robot1.setRotVel(0);
            //             robot1.setVel(-50);
            //             robot2.setRotVel(0);
            //             robot2.setVel(-50);
            //             ArUtil::sleep(5000);
            //             robot1.setVel(0);
            //             robot2.setVel(0);
            //         }
            //     }
            //     else if(command=="stop")
            //     {
            //         robot1.setRotVel(0);
            //         robot1.setVel(0);
            //         robot2.setRotVel(0);
            //         robot2.setVel(0);
            //     }
            //     precommand = myQue.front();
            //     myQue.pop();
            // }
            else
            {
                myQue.pop();
                myQue.pop();
            }
        }else
        {
            while(!myQue.empty())
            {
                myQue.pop();
            }
        }
        ArUtil::sleep(500);
    }
    return NULL;
}

int main(int argc, char *argv[])
{

    config = cmd_ln_init(NULL, ps_args(), TRUE,
                         "-hmm", MODELDIR "/en-us/en-us",
                         "-lm", MODELDIR "/en-us/en-us.lm.bin",
                         "-dict", MODELDIR "/en-us/cmudict-en-us.dict",
                         NULL);
    if (config == NULL)
        return 1;

    ps = ps_init(config);
    if (ps == NULL)
        return 1;

    Aria::init();
    ArArgumentParser parser1(&argc, argv);
    parser1.addDefaultArgument("-rh 10.0.126.18");

    ArRobotConnector robotConnector1(&parser1, &robot1);

    if (!robotConnector1.connectRobot())
    {
        if (!parser1.checkHelpAndWarnUnparsed())
        {
            ArLog::log(ArLog::Terse, "Could not connect to robot, will not have parameter file so options displayed later may not include everything");
        }
        else
        {
            ArLog::log(ArLog::Terse, "Error, could not connect to robot.");
            Aria::logOptions();
            Aria::exit(1);
        }
    }

    if(!robot1.isConnected())
    {
        ArLog::log(ArLog::Terse, "Internal error: robot connector succeeded but ArRobot::isConnected() is false!");
    }

    ArLaserConnector laserConnector1(&parser1, &robot1, &robotConnector1);

    ArCompassConnector compassConnector1(&parser1);

    if (!Aria::parseArgs() || !parser1.checkHelpAndWarnUnparsed())
    {
        Aria::logOptions();
        Aria::exit(1);
        return 1;
    }

    robot1.addRangeDevice(&sonar);
    robot1.runAsync(true);
    //
    // Aria::init();
    // ArArgumentParser parser2(&argc, argv);
    // parser2.addDefaultArgument("-rh 10.0.126.13");
    //
    // ArRobotConnector robotConnector2(&parser2, &robot2);
    //
    // if (!robotConnector2.connectRobot())
    // {
    //     if (!parser2.checkHelpAndWarnUnparsed())
    //     {
    //         ArLog::log(ArLog::Terse, "Could not connect to robot, will not have parameter file so options displayed later may not include everything");
    //     }
    //     else
    //     {
    //         ArLog::log(ArLog::Terse, "Error, could not connect to robot.");
    //         Aria::logOptions();
    //         Aria::exit(1);
    //     }
    // }
    //
    // if(!robot2.isConnected())
    // {
    //     ArLog::log(ArLog::Terse, "Internal error: robot connector succeeded but ArRobot::isConnected() is false!");
    // }
    //
    // ArLaserConnector laserConnector2(&parser2, &robot2, &robotConnector2);
    //
    // ArCompassConnector compassConnector2(&parser2);
    //
    // if (!Aria::parseArgs() || !parser2.checkHelpAndWarnUnparsed())
    // {
    //     Aria::logOptions();
    //     Aria::exit(1);
    //     return 1;
    // }
    //
    //robot2.addRangeDevice(&sonar);
    //robot2.runAsync(true);


    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, recognize_from_microphone, NULL);
    pthread_create(&thread2, NULL, Robot, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    ps_free(ps);
    Aria::exit(0);
    return 0;
}
