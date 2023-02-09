#include <iostream>
#include <backend.h>
#include <string.h>
#include <serial.h>
#include <pthread.h>

std::vector<User> USERS;

// User lock sluzi za multi-thread access za USERS
// Ako jedan thread upravlja sa listom, da drugi ne pokusava
bool user_lock = false;

// Has wifi sluzi za provjeru wifi-a
// Ako nema wifi, pa ponovo dođe, onda sve usere synca sa bazom
bool has_wifi = false;

// Ova funkcija se izvrsava na posebnom thread-u
// Sluzi za odjavljivanje usera nakon pola 8
// To je u slucaju da se neko zaboravi odjaviti
void *t_smjena(void *)
{
    while (1)
    {
        time_t tt = atot(getTimeNow());
        auto tm = *localtime(&tt);
        if (tm.tm_hour == 19 && tm.tm_min >= 30)
        {
            if (user_lock)
                continue;
            user_lock = true;
            for (auto &user : USERS)
            {
                if (user.isPresent)
                {
                    std::cout << "Logging out " << user.ime << std::endl;
                    db::addUserRecord(&user);
                }
                db::userSync(&user);
            }
            db::recordUsers(USERS);
            user_lock = false;
        }
        sleep(60);
    }
    return nullptr;
}

int main()
{
    // Pocetni fetch za usere.
    USERS = db::getUsers();
    if (USERS.empty())
        return 0;
    db::recordUsers(USERS);

    // Otvaranje usb porta za komunikaciju sa arduinom
    if (serial::openPort("/dev/ttyACM0"))
    {
        std::cout << "USB OPEN ERROR" << std::endl;
        return 0;
    }
    // Bad read sluzi za cooldown da ne pišti stalno
    std::string badRead = getTimeNow();
    while (1)
    {
        // Citaj trenutni tag sa usb
        std::string usb = truncateJSON(serial::readTag());
        if (usb.empty())
            continue;

        bool match = false;
        if (user_lock)
            continue;
        user_lock = true;
        for (auto &user : USERS)
        {
            // Ako trenutni tag postoji u USERS listi
            if (usb.compare(user.tag) == 0)
            {
                match = true;
                // Ovdje provjerava da li su prosle 2 sekunde od proslog skeniranja tog usera
                // To je prevencija duplog citanja
                if (getTimeDiff(user.lastEntry) >= 2)
                {
                    if (db::addUserRecord(&user) == 0)
                    {
                        db::recordUsers(USERS);

                        // Provjeri da li se vratio wifi, i ako jest syncaj sve usere
                        bool newStatus = db::userSync(&user) == 0;
                        if (!has_wifi && newStatus)
                        {
                            for (int i = 0; i < USERS.size(); i++)
                            {
                                if (USERS[i].tag != user.tag)
                                    db::userSync(&USERS[i]);
                            }
                        }
                        has_wifi = newStatus;

                        // Ispisivanje statusa u terminal za debugging i pištanje na arduinu
                        if (user.isPresent)
                        {
                            std::cout << "Dobrodosao " << user.ime << std::endl;
                            serial::usbWriteOK();
                        }
                        else
                        {
                            std::cout << "Vidimo se " << user.ime << std::endl;
                            serial::usbWriteOK();
                        }
                    }
                    else
                        match = false;
                }
                break;
            }
        }
        if (!match && getTimeDiff(badRead) > 1)
        {
            badRead = getTimeNow();
            std::cout << "TAG DID NOT MATCH " << usb << std::endl;
            // Nepoznate kartice upisuj u NEW file
            // Iz tog fajla flutter app cita id i registruje na firebase
            io::writeFile(db::basePath + "NEW", usb);
            serial::usbWriteBAD();
            // Ovdje se ponovo useri fetchaju u slucaju da postoji novi user
            USERS = db::getUsers();
        }
        user_lock = false;
    }
    return 0;
}
