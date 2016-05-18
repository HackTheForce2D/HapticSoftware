#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <SFML/Network.hpp>


//g++ -Wall -c "%f" -lBox2D -lserial -std=c++11

/*
 * Compiler et executer:
g++ -c sfmlPantograph.cpp;
g++ sfmlPantograph.o -o sfml-pantograph -lBox2D -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network; 
./sfml-pantograph
*/

// http://www.unixuser.org/lg/issue74/tougher.html
// http://www.sfml-dev.org/tutorials/2.1/network-socket.php

const double pi = 3.1415926535897;

void xy2angle(sf::Vector2f C, sf::Vector2f A, sf::Vector2f E,
              double a, double b, double c,
             double& OAB, double& OED,sf::Vector2f &B,sf::Vector2f &D);
void angle2xy(double OAB, double OED, double a, double b, double c,
               sf::Vector2f A,sf::Vector2f E,
               sf::Vector2f &B,sf::Vector2f &D,sf::Vector2f &C);
b2Vec2 vec2b2vec(sf::Vector2f V){
	return b2Vec2(V.x,V.y);
	}
sf::Vector2f b2vec2sfvec(b2Vec2 V){
	return sf::Vector2f(V.x,V.y);
	}
struct Parametres
	{
		double *OAB, *OED, a, b, c;
        sf::Vector2f A, E, *B, *D, *C,*force,*couple;
        int *tauxHaptique;
	};
void communicationPC(Parametres p);


int main()
{
	
	//-----------Compteurs pour mesurer la fréquence de calcul----------
	int boucleGraphique(0),tauxGraphique(0),tauxHaptique(0);
	sf::Clock horlogeGraphique, horlogeHaptique;
	//--------------------fenêtre graphique--------------------------------
    sf::RenderWindow window(sf::VideoMode(400, 350),"Haptics");
    window.setPosition(sf::Vector2i(100,50));
    std::ostringstream texteSurImage;
    //------------paramètres du patographe en mm--------------------------
    //double a(40),b(100),c(125),ymin(50),f(177),g(100),OAB(0),OED(0);
    const double a(60),b(100),c(135),ymin(70),f(177),g(100);
    double OAB(0),OED(0);
	sf::Vector2f A(-a/2,10),Bphys(0,0),B(0,0),Cphys(0,0),C(0,0),Dphys(0,0),D(0,0),E(a/2,10);
	sf::Vector2f origine(200,0);
	//-------------------------------------------------------------------
	sf::Vector2f force(0,0); //Newton
	sf::Vector2f couple(0,0); 
	A = A+origine; E = E+origine;
	//---------- Texte à afficher sur l'espace de travail-----------------
    sf::Text textTauxGr, textTauxHa, textCoupleM1, textCoupleM2;
    sf::Font font;
    font.loadFromFile("DejaVuSans.ttf");
    textTauxGr.setPosition(10,10); textTauxGr.setFont(font); textTauxGr.setCharacterSize(9);
    textTauxHa.setPosition(10,18); textTauxHa.setFont(font); textTauxHa.setCharacterSize(9);
    textCoupleM1.setPosition(A.x-30,A.y); textCoupleM1.setFont(font);textCoupleM1.setCharacterSize(11);
    textCoupleM2.setPosition(E.x+30,E.y); textCoupleM2.setFont(font); textCoupleM2.setCharacterSize(11);
	//------------------------------------------------------------------
	sf::VertexArray pantograph(sf::LinesStrip, 5);
	//-----------------------------------------------------------------
	sf::Transform echelle;
	echelle.scale(2, 2, origine.x, origine.y);
	sf::Transform echelle_inverse(echelle.getInverse());
	sf::Transform box2sfml;
	box2sfml.translate(200, 150);
	box2sfml.scale(-10, -10);	
	sf::Transform sfml2box(box2sfml.getInverse());
	
	//----------------Thread du retour d'effort------------------------
	Parametres p = {&OAB,&OED,a,b,c,A,E,&B,&D,&C,
		            &force,&couple,&tauxHaptique};
    sf::Thread threadHaptique(&communicationPC,p);
    threadHaptique.launch();
    
    //------------------------espace de travail-------------------------
    sf::VertexArray espaceTravail(sf::LinesStrip, 5);
    espaceTravail[0].position = echelle.transformPoint(origine + sf::Vector2f(-f/2,ymin));
	espaceTravail[1].position = echelle.transformPoint(origine + sf::Vector2f(f/2,ymin));
	espaceTravail[2].position = echelle.transformPoint(origine + sf::Vector2f(f/2,ymin+g));
	espaceTravail[3].position = echelle.transformPoint(origine + sf::Vector2f(-f/2,ymin+g));
	espaceTravail[4].position = espaceTravail[0].position;
    
    //----------------Boucle Graphique---------------------------------
    while (window.isOpen())
    {
		//Mesurer la taux graphique (doit être 60Hz)
		if(horlogeGraphique.getElapsedTime().asMilliseconds() > 1000){
			horlogeGraphique.restart();
			tauxGraphique = boucleGraphique;
			boucleGraphique = 0;
		}else{
			boucleGraphique++;
		}
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed ||event.type == sf::Event::KeyPressed){
				threadHaptique.terminate();
				window.close();
				}else if(event.type == sf::Event::MouseMoved){				
				//-----------affichage de la position physique du pantographe------
				Cphys = echelle_inverse.transformPoint(event.mouseMove.x,event.mouseMove.y);			
				xy2angle(Cphys,A,E,a,b,c,OAB,OED,Bphys,Dphys);	
				pantograph[0].position = echelle.transformPoint(A);
				pantograph[1].position = echelle.transformPoint(Bphys);
				pantograph[2].position = echelle.transformPoint(Cphys);
				pantograph[3].position = echelle.transformPoint(Dphys);
				pantograph[4].position = echelle.transformPoint(E);
				
			//--------------------------------------------------------------------		
			}else if(event.type == sf::Event::MouseButtonPressed){
			std::cout << "Taux graphique : "  << tauxGraphique << " Hz" << std::endl;
			std::cout <<  "Taux haptique : " << tauxHaptique << " Hz" << std::endl;
			std::cout << "Couple M1 : " << couple.x << " mNm" << std::endl;
			std::cout << "Couple M2 : " << couple.y << " mNm" << std::endl;
			std::cout << "OAB : " << OAB*180/pi << "°" << std::endl;
			std::cout << "OED : " << OED*180/pi << "°" << std::endl;
			std::cout << "X : " << Cphys.x << "mm" << std::endl;
			std::cout << "Y : " << Cphys.y << "mm" << std::endl;
			std::cout << "Force : " << force.x << "N en x, " << force.y << "N en y" << std::endl;
			}
		}
		texteSurImage.str(std::string());
		texteSurImage << "Frame rate : " << tauxGraphique << " Hz";
		textTauxGr.setString(texteSurImage.str());
		texteSurImage.str(std::string());
		texteSurImage << "Haptic feedback rate : " << tauxHaptique << " Hz";
		textTauxHa.setString(texteSurImage.str());
		texteSurImage.str(std::string());
		texteSurImage << std::fixed << std::setprecision(1) << couple.x << " mNm";
		textCoupleM1.setString(texteSurImage.str());
		texteSurImage.str(std::string());
		texteSurImage << std::fixed << std::setprecision(1) << couple.y << " mNm";
		textCoupleM2.setString(texteSurImage.str());
	    window.clear();
        window.draw(pantograph);
        window.draw(espaceTravail);
        window.draw(textTauxGr);
        window.draw(textTauxHa);
        window.draw(textCoupleM1);
        window.draw(textCoupleM2);
        window.display();
    }

    return 0;
}

double norm(sf::Vector2f v){
	return sqrt(v.x*v.x + v.y*v.y);
}

//Calculer les angles à partir de la position de C (teste avec souris)
void xy2angle(sf::Vector2f C, sf::Vector2f A, sf::Vector2f E,
              double a, double b, double c,
             double& OAB, double& OED,sf::Vector2f &B,sf::Vector2f &D)
    {
	//distances entre les moteurs et le point C
	double AC = norm(C-A),
		   EC = norm(C-E);
    if(AC<c-b or EC < c-b) return; // mechaniquement impossible
	double OAC(acos((C.x-A.x)/AC)),
		   OEC(acos((C.x-E.x)/EC));
	// Loi des cosinus pour trouver les angles entre les bras à gauche(ABC) et à droite(EDC)
	double ABC(acos(-(AC*AC-b*b - c*c)/(2*b*c))),
		   EDC(acos(-(EC*EC-b*b - c*c)/(2*b*c)));
	// Loi des sinus pour trouver les angles OAB et OED
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

void communicationPC(Parametres p){
	double OAB(0), OED(0);
	sf::Vector2f A(p.A), E(p.E), B(0,0),D(0,0), C(0,0),
	            force, couple(0,0);
	int boucleHaptique(0), tauxHaptique(0);
    sf::Clock horlogeHaptique;
    sf::Time delai;
    delai = sf::microseconds(200);
    
    //comm setup
    sf::TcpListener listener;

	// bind the listener to a port
	
	if (listener.listen(53200) != sf::Socket::Done)
	{
     error("ERROR on binding");
	}
	
	sf::TcpSocket client;
	while(1){
	if (listener.accept(client) != sf::Socket::Done)
	{
    error("ERROR on accept");
	}
	unsigned char s[10] = "A0000000",r[5] = "0000";
    sf::Packet angles;
    sf::Packet retour;
    //fin comm
    //int encodedAngle(0); //test incrementing
    int status = sf::Socket::Done;
    while(status == sf::Socket::Done){
		if(horlogeHaptique.getElapsedTime().asMilliseconds() > 1000){
			horlogeHaptique.restart();
			tauxHaptique = boucleHaptique;			
			boucleHaptique = 0;
			*p.tauxHaptique = tauxHaptique;
	    }else{
			boucleHaptique++;
		}
		OAB = *p.OAB; if(OAB<0) OAB += 2*pi;
		OED = *p.OED; if(OED<0) OED += 2*pi;
		angles.clear();
		//avec Qt
		//encodedAngle+=50;
		//encodedAngle &= 4095;
		int encodedAngle(0);
		int offset1(1000), offset2(2500);
		encodedAngle = (int)(OAB*4096/(2*pi));
		encodedAngle = (encodedAngle + offset1)%4096;
		s[0] = (char)(encodedAngle >> 8)&255;
		s[1] = (char)(encodedAngle)&255;
		encodedAngle = (int)(OED*4096/(2*pi));
		encodedAngle = (encodedAngle + offset2)%4096;
		s[2] = (char)(encodedAngle >> 8)&255;
		s[3] = (char)(encodedAngle)&255;
		//angles << s;
		//angles << OAB << OED;
		// TCP socket:
		//if(client.send(angles) != sf::Socket::Done)
		status = client.send(&s,8);
		if(status != sf::Socket::Done)
		{
			//error("ERROR sending data");
			std::cout << "error writing" << std::endl;
			client.disconnect();
			couple.x = 0;
		    couple.y = 0;
		}//else std::cout << "sent" << std::endl;
		size_t bytesReceived;
		status = client.receive(r,4,bytesReceived);
		if (status != sf::Socket::Done)
        {
		 //error("ERROR reading from socket");
		 std::cout << "error reading" << std::endl;
		 client.disconnect();
		 couple.x = 0;
		 couple.y = 0;
		}        
		else
		{
			/*
			int decodedTorque;
			decodedTorque = (int)r[0];
			decodedTorque = (decodedTorque << 8) | (int)r[1];
			couple.x = (float) (decodedTorque &511);
			if((decodedTorque >> 15) == 1) couple.x = -couple.x; 
			decodedTorque = (int)r[2];
			decodedTorque = (decodedTorque << 8) | (int)r[3];
			couple.y = (float) (decodedTorque &511);
			if((decodedTorque >> 15) == 1) couple.y = -couple.y; 
			 */
			 int decodedTorque;
			decodedTorque = (int)r[0];
			decodedTorque = (decodedTorque << 8) | (int)r[1];
			couple.x = (float) (decodedTorque &1023) - 500; 
			decodedTorque = (int)r[2];
			decodedTorque = (decodedTorque << 8) | (int)r[3];
			couple.y = (float) (decodedTorque &1023) - 500; 
		}
		/*
		retour.clear();
		if (client.receive(retour) != sf::Socket::Done)
        {
		 error("ERROR reading from socket");
		}        
		else{
		//retour >> couple.x >> couple.y;	
		
	    }*/
		*p.couple = couple;

	    sf::sleep(delai);
	}
}
}
