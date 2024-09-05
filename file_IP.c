#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define MAX_CHAR 250

void head() 
{
    printf("Content-Type: text/html\n\n");
    printf("<html><head><meta charset=\"UTF-8\" /><title>IP</title></head><body>");
}

char *compare_ip(int a, int b, int c, int d) 
{
    static char class[20];
    if (a < 0 || a > 255 || b < 0 
        || b > 255 || c < 0 ||
        c > 255 || d < 0 || d > 255) 
    {
        strcpy(class, "IP Invalide");
    } 
    else if (a >= 0 && a <= 127) 
    {
        strcpy(class, "class A");
    } 
    else if (a >= 128 && a <= 191) 
    {
        strcpy(class, "class B");
    } 
    else if (a >= 192 && a <= 223) 
    {
        strcpy(class, "class C");
    } 
    else if (a >= 224 && a <= 239) 
    {
        strcpy(class, "class D");
    } 
    else if (a >= 240 && a <= 255) 
    {
        strcpy(class, "class E");
    } 
    else 
    {
        strcpy(class, "IP Invalide");
    }
    return class;
}

char *msr_par_defaut(int a, int b, int c, int d) 
{
    static char msr[20];
    char *classe = compare_ip(a, b, c, d);
    if (strcmp(classe, "class A") == 0) 
    {
        strcpy(msr, "255.0.0.0");
    } 
    else if (strcmp(classe, "class B") == 0) 
    {
        strcpy(msr, "255.255.0.0");
    } 
    else if (strcmp(classe, "class C") == 0) 
    {
        strcpy(msr, "255.255.255.0");
    } 
    else 
    {
        strcpy(msr, "Inconnu");
    }
    return msr;
}

void gestion_reseau(int a, int b, int c, int d) 
{
    char add_res[MAX_CHAR];
    char *classe = compare_ip(a, b, c, d);

    if (strcmp(classe, "class A") == 0) 
    {
        strcpy(add_res, "Réseau A");
    } 
    else if (strcmp(classe, "class B") == 0) 
    {
        strcpy(add_res, "Réseau B");
    } 
    else if (strcmp(classe, "class C") == 0) 
    {
        strcpy(add_res, "Réseau C");
    } 
    else 
    {
        strcpy(add_res, "Classe non prise en charge");
        printf("<p>%s</p>", add_res);
    }
}

void calcul_adresses(int a, int b, int c, int d, const char *msr, char *reseau, char *broadcast) 
{
    int msr_parts[4];
    int ip_parts[4] = {a, b, c, d};
    int reseau_parts[4], broadcast_parts[4];

    // Convertir le masque de sous-réseau en entiers
    sscanf(msr, "%d.%d.%d.%d", &msr_parts[0], &msr_parts[1], &msr_parts[2], &msr_parts[3]);

    // Calculer l'adresse réseau
    for (int i = 0; i < 4; i++) 
    {
        reseau_parts[i] = ip_parts[i] & msr_parts[i];
    }

    // Calculer l'adresse de diffusion
    for (int i = 0; i < 4; i++) 
    {
        broadcast_parts[i] = ip_parts[i] | (~msr_parts[i] & 255);
    }

    // Formater les adresses en chaînes de caractères
    sprintf(reseau, "%d.%d.%d.%d", reseau_parts[0], reseau_parts[1], reseau_parts[2], reseau_parts[3]);
    sprintf(broadcast, "%d.%d.%d.%d", broadcast_parts[0], broadcast_parts[1], broadcast_parts[2], broadcast_parts[3]);
}

void get_data() {
    char *data = getenv("QUERY_STRING");
    if (data == NULL) 
    {
        printf("<p>Erreur lors de l'analyse des données.</p>");
        printf("<button><a href=\"index.html\">Réessayer</a></button>");
        printf("</body></html>");
        exit(1);
    }

    char msr[MAX_CHAR];
    char *classe;
    int a, b, c, d;
    char reseau[MAX_CHAR];
    char broadcast[MAX_CHAR];

    // Analyse des données de la requête GET
    int ip_string = sscanf(data, "IP=%d.%d.%d.%d&MSR=%s", &a, &b, &c, &d, msr);

    if (ip_string == 5) 
    {
        char *msr_default = msr_par_defaut(a, b, c, d);
        if (strcmp(msr, msr_default) != 0) 
        {
            printf("<p>Masque de sous-réseau inconnu</p>");
            printf("<button><a href=\"index.html\">Réessayer</a></button>");
            printf("</body></html>");
            exit(1);
        } 
        else 
        {
            classe = compare_ip(a, b, c, d);
            calcul_adresses(a, b, c, d, msr, reseau, broadcast);

            // Affichage dans un tableau HTML
            printf("<table border=\"1\">");
            printf("<tr><td><strong>IP</strong></td><td><strong>MSR</strong></td></tr>");
            printf("<tr><td>%d.%d.%d.%d</td><td>%s</td></tr>", a, b, c, d, msr);
            printf("</table>");

            printf("<table border=\"1\">");
            printf("<tr><td><strong>Type</strong></td><td><strong>Adresse</strong></td></tr>");
            printf("<tr><td>Adresse Réseau</td><td>%s</td></tr>", reseau);
            printf("<tr><td>Adresse de Diffusion</td><td>%s</td></tr>", broadcast);
            printf("</table>");
            
            printf("<p><strong>Classe:</strong> %s</p>", classe);
            
            gestion_reseau(a, b, c, d);
        }
    } 
    else 
    {
        printf("<p>Adresse IP invalide</p>");
        printf("<button><a href=\"index.html\">Réessayer</a></button>");
    }

    printf("</body></html>");
}
