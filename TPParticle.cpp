



// C'est un idle. J'ai une money qui sont en fait les rebonds des particules. Au départ
//Il y a deux boules qui se déplacent et me donne de l'argent en rebondissant sur les murs et avec cet argent,
//Je vais pouvoir acheter de nouvelles boules, qui donnent + d'argent et qui ont d'autre particularités.



#include<Grapic.h>
#include <math.h>


using namespace grapic;
using namespace std ;

struct Vec2 { float x, y ; };
const int DIMW = 1000 ;
const int MAX = 40 ;
const int MAX_BOUTON = 30 ;
const int DIMY_BOUTON = 60 ;
const int DIMX_BOUTON = 150 ;

struct Color
{
    int r, g, b ;
};

Color make_color(int r, int g, int b)
{
    Color res ;
    res.r = r ;
    res.g = g ;
    res.b = b ;
    return res ;
}

Vec2 make_Vec2(float x, float y)
{
    struct Vec2 z ;
    z.x = x ;
    z.y = y ;
    return z ;
}

float toDeg(float B)
{
    return B*180/M_PI ;
}

float toRad(float B)
{
    return B*M_PI/180 ;
}

Vec2 make_Vec2_exp(float r, float theta)
{
    struct Vec2 z ;
    theta = toRad(theta) ;
    z.x = r*cos(theta) ;
    z.y = r*sin(theta) ;
    return z ;
}

Vec2 operator+(Vec2 a, Vec2 b)
{
    struct Vec2 res ;
    res.x = a.x + b.x ;
    res.y = a.y + b.y ;
    return res ;
}

Vec2 operator+(Vec2 a, int b)
{
    struct Vec2 res ;
    res.x = a.x + b ;
    res.y = a.y + b ;
    return res ;
}

Vec2 operator-(Vec2 a, Vec2 b)
{
    struct Vec2 res ;
    res.x = a.x - b.x ;
    res.y = a.y - b.y ;
    return res ;
}

Vec2 operator*(float lambda, Vec2 b)
{ return make_Vec2(lambda*b.x, lambda*b.y) ; }

Vec2 operator*(Vec2 b, float lambda)
{ return make_Vec2(lambda*b.x, lambda*b.y) ; }

Vec2 scale(Vec2 p, float cx, float cy, float lambda)
{
    Vec2 res ;
    Vec2 refe = make_Vec2(cx, cy) ;
    res = lambda*(p-refe)+refe ;
    return res ;
}

Vec2 operator*(Vec2 a, Vec2 b)
{
    Vec2 res ;
    res.x = a.x*b.x - a.y*b.y ;
    res.y = a.x*b.y + a.y*b.x ;
    return res ;
}

Vec2 operator/(Vec2 a, float b)
{
    Vec2 res ;
    res.x = a.x/b ;
    res.y = a.y/b ;
    return res ;
}

Vec2 rotation(Vec2 p, float cx, float cy, float theta_deg)
{
    Vec2 res ;
    Vec2 refe = make_Vec2(cx, cy) ;
    Vec2 rot = make_Vec2_exp(1, theta_deg) ;
    res = (p-refe)*rot+refe ;
    return res ;
}

struct Particule
{
    Vec2 sommeForces, acceleration, vitesse, position ;
    float masse ;
    Color couleur ;
    int taille, valeur ;
};

struct Bouton
{
    int nbAchete, numero, prix ;
    Vec2 position ;
    Color couleur ;
    Particule p ;
};

struct AllBouton
{
    Bouton tab[MAX_BOUTON] ;
    int nbBouton ;
};

struct World
{
    int nbP, score ;
    Particule tab[MAX] ;
};

float distance(Vec2 A, Vec2 B)
{
    return sqrtf((B.x-A.x)*(B.x-A.x)+(B.y-A.y)*(B.y-A.y)) ;
}

void enlevePart(World& w, int indice)
{
    if (indice==w.nbP-1)
    {
        w.nbP-- ;
    }
    else
    {
        for (int i=indice;i<w.nbP-1;i++)
        {
            w.tab[i] = w.tab[i+1] ;
        }
        w.nbP-- ;
    }

}

void initBouton(Bouton& B, Vec2 position, int numero)
{
    B.position = position ;
    B.nbAchete = 0 ;
    B.numero = numero ;
}

void initAllBouton(AllBouton& allb)
{
    allb.nbBouton = 3 ;
    for (int i=0;i<allb.nbBouton;i++)
    {
        initBouton(allb.tab[i], make_Vec2(810, 900-i*100), i+1) ;
    }
    allb.tab[0].couleur = make_color(255, 0, 0) ; allb.tab[0].prix = 10 ;
    allb.tab[1].couleur = make_color(0, 255, 0) ; allb.tab[1].prix = 200 ;
    allb.tab[2].couleur = make_color(0, 0, 255) ; allb.tab[2].prix = 5000 ;
}
void drawBouton(Bouton B)
{
    color(0, 0, 0) ;
    rectangle(B.position.x, B.position.y, B.position.x+DIMX_BOUTON, B.position.y+DIMY_BOUTON) ;
    color(B.couleur.r, B.couleur.g, B.couleur.b) ;
    circleFill(B.position.x+20, B.position.y+40, 10) ;
    print(B.position.x+70, B.position.y+26, B.nbAchete) ;
    print(B.position.x+30, B.position.y+3, "Prix :") ;
    print(B.position.x+80, B.position.y+3, B.prix) ;
}

void drawAllBouton(AllBouton allb)
{
    for (int i=0;i<allb.nbBouton;i++)
    {
        drawBouton(allb.tab[i]) ;
    }
}

void partInit(Particule& point, Vec2 pos, Vec2 vit, float m, Color couleur, int valeur, int taille)
{
    point.sommeForces = make_Vec2(0, 0);
    point.acceleration = make_Vec2(0, 0);
    point.position = pos ;
    point.vitesse = vit;
    point.masse = m ;
    point.valeur = valeur ;
    point.couleur = couleur ;
    point.taille = taille ;
}

void partAddForce(Particule& part, Vec2 force)
{
    part.sommeForces = part.sommeForces + force ;
}

void partUpdatePV(World& w, Particule& part, float dt, float FRICTION)
{
    part.acceleration = part.sommeForces/part.masse ;
    part.vitesse = part.vitesse + part.acceleration * dt ;
    part.position = part.position + part.vitesse * dt ;
    part.sommeForces = make_Vec2(0, 0);
    if (part.position.y < 0 || part.position.y > DIMW)
    {
        w.score += part.valeur ;
        if (part.taille == 20)
        {
            for (int i = 0;i<w.nbP;i++)
            {
                if ((w.tab[i].taille != 20) && distance(part.position+10, w.tab[i].position+(w.tab[i].taille/2)) <= 200)
                {
                    part.valeur += w.tab[i].valeur ;
                    enlevePart(w, i) ;
                }

            }
        }

        part.vitesse.y *= -1 ;
        if (part.position.y < 0)
            {part.position.y *= -1 ;}
        else
        {part.position.y = DIMW - (part.position.y - DIMW);}
        part.vitesse = FRICTION * part.vitesse ;
    }
    else if (part.position.x < 0 || part.position.x > (DIMW-(DIMW/5)))
    {
        w.score += part.valeur ;
        if (part.taille == 20)
        {
            for (int i = 0;i<w.nbP;i++)
            {
                if ((w.tab[i].taille != 20) && distance(part.position+10, w.tab[i].position+(w.tab[i].taille/2)) <= 200)
                {
                    part.valeur += w.tab[i].valeur ;
                    enlevePart(w, i) ;
                }

            }
        }
        part.vitesse.x *= -1 ;
        if (part.position.x < 0)
            {part.position.x *= -1 ;}
        else
        {part.position.x = (DIMW-(DIMW/5)) - (part.position.x - (DIMW-(DIMW/5)));}
        part.vitesse = FRICTION * part.vitesse ;
    }
}

void partDraw(Particule& part)
{
    color(part.couleur.r, part.couleur.g, part.couleur.b) ;
    circleFill(part.position.x, part.position.y, part.taille) ;
}


World init_world(World& monde)
{
    monde.nbP = 0 ;
    monde.score = 10000000 ;
}

void worldDraw(World& w)
{
     for (int i = 0; i<w.nbP; i++)
     {
        partDraw(w.tab[i]) ;
        partUpdatePV(w, w.tab[i], 0.005, 1) ;
     }
     color(0, 0, 0) ;
     print(820, 100, "Argent :") ;
     print(900, 100, w.score) ;


}

void clickBouton(AllBouton& allb, World& w)
{
    int x=0, y=0 ;
    while (isMousePressed(SDL_BUTTON_LEFT))
        {mousePos(x, y) ; cout<<x<<y<<endl ;}

    for (int i=0;i<allb.nbBouton;i++)
    {
        if ((x >= allb.tab[i].position.x) && (y >= allb.tab[i].position.y) && (x <= allb.tab[i].position.x+DIMX_BOUTON) && (y <= allb.tab[i].position.y+DIMY_BOUTON))
            {
                switch (allb.tab[i].numero)
                {
                    case 1 :
                            if (w.score >= allb.tab[i].prix)
                            {
                                partInit(w.tab[w.nbP], make_Vec2(DIMW/2, DIMW/2), make_Vec2(150, 150)*make_Vec2_exp(1, frand(0, 360)), 10, allb.tab[i].couleur, 1, 10) ;
                                w.nbP++ ;  allb.tab[i].prix = 0.6*pow(allb.tab[i].nbAchete, 1.5)+10 ; w.score -= allb.tab[i].prix ; allb.tab[i].nbAchete++ ;
                            } break ;

                    case 2 : if (w.score >= allb.tab[i].prix)
                            {
                                partInit(w.tab[w.nbP], make_Vec2(DIMW/2, DIMW/2), make_Vec2(300, 300)*make_Vec2_exp(1, frand(0, 360)), 10, allb.tab[i].couleur, 1, 10) ;
                                w.nbP++ ;  allb.tab[i].prix = 50*pow(allb.tab[i].nbAchete+1, 1.6)+150 ; w.score -= allb.tab[i].prix ; allb.tab[i].nbAchete++ ;
                            } break ;
                    case 3 : if (w.score >= allb.tab[i].prix)
                            {
                                partInit(w.tab[w.nbP], make_Vec2(DIMW/2, DIMW/2), make_Vec2(120, 120)*make_Vec2_exp(1, frand(0, 360)), 20, allb.tab[i].couleur, 20, 20) ;
                                w.nbP++ ; allb.tab[i].prix *= 5 ;  w.score -= allb.tab[i].prix ; allb.tab[i].nbAchete++ ;
                            } break ;
                    default : break ;
                }
            }
    }

}



int main(int , char ** )
{

 bool stop=false;
 winInit("Particles", DIMW, DIMW);
 backgroundColor(255, 255, 255);
 World w ;
 init_world(w);
 AllBouton allb ;
 initAllBouton(allb) ;
 while( !stop )
 {
 winClear();
 color(0, 0, 0) ;
 line(DIMW-DIMW/5, 0, DIMW-DIMW/5, DIMW) ;
 worldDraw(w) ;
 drawAllBouton(allb) ;
 clickBouton(allb, w) ;
 stop = winDisplay();
 }
 winQuit();
 return 0;
}
