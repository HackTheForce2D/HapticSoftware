#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <SFML/Network.hpp>
#define TCP_PORT 53200

const double pi = 3.1415926535897;

void xy2angle(sf::Vector2f C, sf::Vector2f A, sf::Vector2f E,
              double a, double b, double c,
             double& OAB, double& OED,sf::Vector2f &B,sf::Vector2f &D);

struct Parameters
    {
        double *OAB, *OED, a, b, c;
        sf::Vector2f A, E, *B, *D, *C,*force,*torque;
        int *hapticRate;
    };
void communicationPC(Parameters p);


int main()
{
    
    //-----------Counters to mesure thread rates------------------------
    int graphicLoop(0),frameRate(0),hapticRate(0);
    sf::Clock graphicCounter, horlogeHaptique;
    //--------------------SFML window-----------------------------------
    sf::RenderWindow window(sf::VideoMode(400, 450),"Haptics");
    window.setPosition(sf::Vector2i(100,50));
    std::ostringstream textOverlay;
    //------------Pantograph parameters in mm---------------------------
    //double a(40),b(100),c(125),ymin(50),f(177),g(100),OAB(0),OED(0);
    const double a(60),b(100),c(135),ymin(90),f(177),g(100);
    double OAB(0),OED(0);
    sf::Vector2f A(-a/2,10),Bphys(0,0),B(0,0),Cphys(0,0),C(0,0),
    Dphys(0,0),D(0,0),E(a/2,10), origin(200,0);
    //------------------------------------------------------------------
    sf::Vector2f force(0,0); //Newton
    sf::Vector2f torque(0,0); 
    A = A+origin; E = E+origin;
    //---------- Text displayed on the SFML window----------------------
    sf::Text textRateGr, textRateHa, textTorqueM1, textTorqueM2;
    sf::Font font;
    font.loadFromFile("DejaVuSans.ttf");
    textRateGr.setPosition(10,10); textRateGr.setFont(font); 
    textRateGr.setCharacterSize(9);
    textRateHa.setPosition(10,18); textRateHa.setFont(font); 
    textRateHa.setCharacterSize(9);
    textTorqueM1.setPosition(A.x-30,A.y); textTorqueM1.setFont(font);
    textTorqueM1.setCharacterSize(11);
    textTorqueM2.setPosition(E.x+30,E.y); textTorqueM2.setFont(font); 
    textTorqueM2.setCharacterSize(11);
    //------------------------------------------------------------------
    sf::VertexArray pantograph(sf::LinesStrip, 5);
    //------------------------------------------------------------------
    sf::Transform scaleFactor;
    scaleFactor.scale(2, 2, origin.x, origin.y);
    sf::Transform scaleFactor_inverse(scaleFactor.getInverse());
    sf::Transform box2sfml;
    box2sfml.translate(200, 150);
    box2sfml.scale(-10, -10);   
    sf::Transform sfml2box(box2sfml.getInverse());
    
    //----------------Force feedback thread-----------------------------
    Parameters p = {&OAB,&OED,a,b,c,A,E,&B,&D,&C,
                    &force,&torque,&hapticRate};
    sf::Thread hapticThread(&communicationPC,p);
    hapticThread.launch();
    
    //------------------------espace de travail-------------------------
    sf::VertexArray workSpace(sf::LinesStrip, 5);
    workSpace[0].position = scaleFactor.transformPoint
                            (origin + sf::Vector2f(-f/2,ymin));
    workSpace[1].position = scaleFactor.transformPoint
                            (origin + sf::Vector2f(f/2,ymin));
    workSpace[2].position = scaleFactor.transformPoint
                            (origin + sf::Vector2f(f/2,ymin+g));
    workSpace[3].position = scaleFactor.transformPoint
                            (origin + sf::Vector2f(-f/2,ymin+g));
    workSpace[4].position = workSpace[0].position;
    
    //----------------Graphics thread-----------------------------------
    while (window.isOpen())
    {
        //Mesure the frame rate (should be 60Hz on most computers)
        if(graphicCounter.getElapsedTime().asMilliseconds() > 1000){
            graphicCounter.restart();
            frameRate = graphicLoop;
            graphicLoop = 0;
        }else{
            graphicLoop++;
        }
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed ||
                event.type == sf::Event::KeyPressed){
                hapticThread.terminate();
                window.close();
                }else if(event.type == sf::Event::MouseMoved){
                //-----------Display the Pantograph's 5 bars------------
                Cphys = scaleFactor_inverse.transformPoint
                        (event.mouseMove.x,event.mouseMove.y);
                xy2angle(Cphys,A,E,a,b,c,OAB,OED,Bphys,Dphys);
                pantograph[0].position = scaleFactor.transformPoint(A);
                pantograph[1].position = scaleFactor.
                                           transformPoint(Bphys);
                pantograph[2].position = scaleFactor.
                                           transformPoint(Cphys);
                pantograph[3].position = scaleFactor.
                                           transformPoint(Dphys);
                pantograph[4].position = scaleFactor.transformPoint(E);
                
            //----------------------------------------------------------
            }else if(event.type == sf::Event::MouseButtonPressed){
            std::cout << "Frame rate : "  << frameRate 
                      << " Hz" << std::endl;
            std::cout <<  "Haptic feedback rate : " << hapticRate 
                      << " Hz" << std::endl;
            std::cout << "Torque M1 : " << torque.x 
                      << " mNm" << std::endl;
            std::cout << "Torque M2 : " << torque.y 
                      << " mNm" << std::endl;
            std::cout << "OAB : " << OAB*180/pi << "°" << std::endl;
            std::cout << "OED : " << OED*180/pi << "°" << std::endl;
            std::cout << "X : " << Cphys.x << "mm" << std::endl;
            std::cout << "Y : " << Cphys.y << "mm" << std::endl;
            std::cout << "Force : " << force.x << "N en x, " 
                                    << force.y << "N en y" << std::endl;
            }
        }
        textOverlay.str(std::string());
        textOverlay << "Frame rate : " << frameRate << " Hz";
        textRateGr.setString(textOverlay.str());
        textOverlay.str(std::string());
        textOverlay << "Haptic feedback rate : " << hapticRate << " Hz";
        textRateHa.setString(textOverlay.str());
        textOverlay.str(std::string());
        textOverlay << std::fixed << std::setprecision(1) 
                    << torque.x << " mNm";
        textTorqueM1.setString(textOverlay.str());
        textOverlay.str(std::string());
        textOverlay << std::fixed << std::setprecision(1) 
                    << torque.y << " mNm";
        textTorqueM2.setString(textOverlay.str());
        window.clear();
        window.draw(pantograph);
        window.draw(workSpace);
        window.draw(textRateGr);
        window.draw(textRateHa);
        window.draw(textTorqueM1);
        window.draw(textTorqueM2);
        window.display();
    }

    return 0;
}

double norm(sf::Vector2f v){
    return sqrt(v.x*v.x + v.y*v.y);
}

// Calculate encoder rotation from end-effector position
// (Inverse geometric model)
void xy2angle(sf::Vector2f C, sf::Vector2f A, sf::Vector2f E,
              double a, double b, double c,
             double& OAB, double& OED,sf::Vector2f &B,sf::Vector2f &D)
    {
    //distances from each motor to the point C
    double AC = norm(C-A),
           EC = norm(C-E);
    if(AC<c-b or EC < c-b) return; // mechanically impossible
    double OAC(acos((C.x-A.x)/AC)),
           OEC(acos((C.x-E.x)/EC));
    // Law of cosines to find the angles between the left bars (ABC)
    // and the right bars (EDC)
    double ABC(acos(-(AC*AC-b*b - c*c)/(2*b*c))),
           EDC(acos(-(EC*EC-b*b - c*c)/(2*b*c)));
    // Law of sines to find OAB and OED
    double ACB(asin(b*sin(ABC)/AC)),
           ECD(asin(b*sin(EDC)/EC));
    double CAB(pi - ACB - ABC),
           CED(pi - ECD - EDC);
    OAB = OAC + CAB;
    OED = OEC - CED;
    B = A+sf::Vector2f(b*cos(OAB),b*sin(OAB));
    D = E+sf::Vector2f(b*cos(OED),b*sin(OED));
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void communicationPC(Parameters p){
    double OAB(0), OED(0);
    sf::Vector2f A(p.A), E(p.E), B(0,0),D(0,0), C(0,0),
                force, torque(0,0);
    int hapticLoop(0), hapticRate(0);
    sf::Clock horlogeHaptique;
    sf::Time tcpDelay;
    tcpDelay = sf::microseconds(200);
    
    // Setup TCP communication
    sf::TcpListener listener;

    // bind the listener to a port
    
    if (listener.listen(TCP_PORT) != sf::Socket::Done)
    {
     error("ERROR on binding");
    }
    
    std::cout << "Listening on port " << TCP_PORT << std::endl;
    sf::TcpSocket client;
    while(1){
    if (listener.accept(client) != sf::Socket::Done)
    {
    error("ERROR on accept");
    }
    unsigned char s[10] = "A0000000",r[5] = "0000";
    int status = sf::Socket::Done;
    while(status == sf::Socket::Done){
        if(horlogeHaptique.getElapsedTime().asMilliseconds() > 1000){
            horlogeHaptique.restart();
            hapticRate = hapticLoop;            
            hapticLoop = 0;
            *p.hapticRate = hapticRate;
        }else{
            hapticLoop++;
        }
        OAB = *p.OAB; if(OAB<0) OAB += 2*pi;
        OED = *p.OED; if(OED<0) OED += 2*pi;
        int encodedAngle(0);
        // Simulate encoder offset to test calibration routine
        int offset1(1000), offset2(2500);
        encodedAngle = (int)(OAB*4096/(2*pi));
        encodedAngle = (encodedAngle + offset1)%4096;
        s[0] = (char)(encodedAngle >> 8)&255;
        s[1] = (char)(encodedAngle)&255;
        encodedAngle = (int)(OED*4096/(2*pi));
        encodedAngle = (encodedAngle + offset2)%4096;
        s[2] = (char)(encodedAngle >> 8)&255;
        s[3] = (char)(encodedAngle)&255;
        status = client.send(&s,8);
        if(status != sf::Socket::Done)
        {
            //error("ERROR sending data");
            std::cout << "error writing" << std::endl;
            client.disconnect();
            torque.x = 0;
            torque.y = 0;
        }
        size_t bytesReceived;
        status = client.receive(r,4,bytesReceived);
        if (status != sf::Socket::Done)
        {
         //error("ERROR reading from socket");
         std::cout << "error reading" << std::endl;
         client.disconnect();
         torque.x = 0;
         torque.y = 0;
        }        
        else
        {
            int decodedTorque;
            decodedTorque = (int)r[0];
            decodedTorque = (decodedTorque << 8) | (int)r[1];
            torque.x = (float) (decodedTorque &1023) - 500; 
            decodedTorque = (int)r[2];
            decodedTorque = (decodedTorque << 8) | (int)r[3];
            torque.y = (float) (decodedTorque &1023) - 500; 
        }

        *p.torque = torque;

        sf::sleep(tcpDelay);
    }
}
}
