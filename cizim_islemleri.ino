//Ek_025120t
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET 4
// Joysticklerin port tanımları
#define p_jx A0
#define p_jy A1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Derece tanımlamaları
signed char current_degree_now = 25, current_degree_last = 25, wanted_degree = 25;
signed char degree_min_max[] = {18, 29};

// Derecelerin ekran üzerindeki yerlesimleri
byte current_degree_loc[] = {90, 19};
byte wanted_degree_loc[] = {47, 19};

// Suanki Fan seviyesi
byte fan_level_default = 4, fan_level_current = 0, fan_level_last = 0;
byte fan_min_max[] = {1, 4};

// Sayfalandirma
byte current_page = 0;
byte max_page = 3;
byte dot_width_height = 4;

// Lokasyoları yazılım ile belirledim ama arduino
// 69 - 75 - 81
byte slider_page_property[][2] = {
    {69,28},
    {75,28},
    {81,28}
};

// Joystick x,y 
int joy_x = 513, joy_y = 513;

// On Off image
const unsigned char on_off_image[] PROGMEM = {
    // 'on_off(copy), 30x32px
    0x00, 0x07, 0x80, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x07, 0x80, 0x00,
    0x01, 0xc7, 0x8e, 0x00, 0x03, 0xc7, 0x8f, 0x00, 0x07, 0xc7, 0x8f, 0x80, 0x0f, 0x87, 0x87, 0xc0,
    0x1f, 0x07, 0x83, 0xe0, 0x3e, 0x07, 0x81, 0xf0, 0x3c, 0x07, 0x80, 0xf0, 0x7c, 0x03, 0x00, 0xf8,
    0x78, 0x00, 0x00, 0x78, 0x78, 0x00, 0x00, 0x78, 0xf0, 0x00, 0x00, 0x3c, 0xf0, 0x00, 0x00, 0x3c,
    0xf0, 0x00, 0x00, 0x3c, 0xf0, 0x00, 0x00, 0x3c, 0xf0, 0x00, 0x00, 0x3c, 0xf0, 0x00, 0x00, 0x3c,
    0xf0, 0x00, 0x00, 0x3c, 0x70, 0x00, 0x00, 0x38, 0x78, 0x00, 0x00, 0x78, 0x7c, 0x00, 0x00, 0xf8,
    0x3e, 0x00, 0x01, 0xf0, 0x3f, 0x00, 0x03, 0xf0, 0x1f, 0x80, 0x07, 0xe0, 0x0f, 0xc0, 0x0f, 0xc0,
    0x07, 0xff, 0xff, 0x80, 0x01, 0xff, 0xfe, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x1f, 0xe0, 0x00};

// Ana ekranda fan seviyeleri ile kullanılan gorsel
const unsigned char main_fan_level[] PROGMEM = {
    0x80, 0xc0, 0xe0, 0xf0, 0xf8};

// Fan ekranında kullanılacak ikonlar

// 'left_fan', 10x12px
const unsigned char left_fan[] PROGMEM = {
    0x18, 0x00, 0x3c, 0x00, 0x7e, 0x00, 0xff, 0x00, 0xff, 0x80, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0,
    0xff, 0xc0, 0x7f, 0x80, 0x3f, 0x00, 0x0e, 0x00};
// 'bottom_fan', 10x10px
const unsigned char bottom_fan[] PROGMEM = {
    0x07, 0xc0, 0x0f, 0xc0, 0x1f, 0xc0, 0x3f, 0xc0, 0x3f, 0xc0, 0x7f, 0xc0, 0x7f, 0xc0, 0xff, 0xc0,
    0xff, 0x80, 0x7f, 0x00};
// 'top_fan', 10x10px
const unsigned char top_fan[] PROGMEM = {
    0x3f, 0x80, 0x7f, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0x80, 0xff, 0x00, 0xfe, 0x00,
    0xfc, 0x00, 0x7c, 0x00};
// 'right_fan', 12x12px
const unsigned char right_fan[] PROGMEM = {
    0x3f, 0x80, 0x7f, 0xc0, 0xff, 0xe0, 0xff, 0xf0, 0xff, 0xf0, 0xff, 0xf0, 0x7f, 0xf0, 0x3f, 0xf0,
    0x3f, 0xf0, 0x1f, 0xe0, 0x0f, 0xc0, 0x07, 0x00};

// 'danger_symbol', 5x29px
const unsigned char danger_symbol[] PROGMEM = {
    0x70, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
    0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0x70, 0x00, 0x70, 0xf8, 0xf8, 0xf8, 0x70, 0x20};

unsigned long eskiZaman = 0;
unsigned long yeniZaman;
void setup()
{
    Serial.begin(9600);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.begin();
    display.clearDisplay();
    display.display();
    
    
    page_boot_draw();
    /*
    delay(2000);
    page_main_draw();
    */

    // page_fan_draw();
    // delay(500);
    // increase_fan();
    // delay(500);
    // increase_fan();
}

void loop()
{
    yeniZaman = millis();
    if(yeniZaman-eskiZaman > 10000 && current_page != 0){
        // 10 saniye
        eskiZaman = yeniZaman;
        
        if(current_degree_now <= 29)
            set_current_degree(current_degree_now+1);
        else
            set_current_degree(20);
        display.display();
        
    }
    

    joy_x = analogRead(p_jx);
    joy_y = analogRead(p_jy);
    
    if(joy_x >= 1000){
        // SAGA BASIS
        next_page();
        while (joy_x > 1000)
        {
            joy_x = analogRead(p_jx);
        }
        delay(10);
    }else if(joy_x <= 20){
        // SOLA BASIS
        prev_page();
        while (joy_x <= 20)
        {
            joy_x = analogRead(p_jx);
        }
        delay(10);
    }

    if(joy_y >= 1000){
        // ALTA BASIS
        switch (current_page)
        {
        case 0:
            current_page = 1;
            page_main_draw();
            break;
        case 1:
            decrease_degree();
            break;
        case 2:
            decrease_fan();
            break;
        case 3:
            page_boot_draw();
            fan_level_current = 0;
            fan_level_last = 0;
        default:
            break;
        }
      
        while (joy_y > 1000)
        {
            joy_y = analogRead(p_jy);
        }
        delay(10);
    }else if(joy_y <= 20){
        // USTE BASIS
        switch (current_page){
        case 1:
            increase_degree();
            break;
        case 2:
            increase_fan();
            break;
        default:
            break;
        }
      
        while (joy_y <= 20)
        {
            joy_y = analogRead(p_jy);
        }
        delay(10);
    }
}

/* --------------------GENEL ISLEMLER BASLANGIC-------------------- */
void write_normal(byte loc_x, byte loc_y, unsigned char color, String text)
{
    /*
    @params:
        loc_x,loc_y => Cursoru konumlandırmak için ekranın x,y koordinatı
        color => Renk
        text => Yazdırılacak yazı
    */
    display.setTextWrap(false);
    display.setCursor(loc_x, loc_y);
    display.setTextColor(color);
    display.setTextSize(1);
    display.print(text);
}

void update_write(byte loc_x, byte loc_y, String old, String new_text)
{
    /*
    @params:
        loc_x,loc_y => Cursoru konumlandırmak için ekranın x,y koordinatı
        color => Renk
        old => Silinecek yazı
        new_text => Yazılacak yazı
    */
    write_normal(loc_x, loc_y, BLACK, old);
    write_normal(loc_x, loc_y, WHITE, new_text);
}

void draw_main_lines()
{
    // Ana ekrandaki cizgilerin cizimi
    display.drawLine(76, 6, 76, 23, WHITE);
    display.drawLine(38, 15, 115, 15, WHITE);
}

void draw_all_dot()
{
    // Ekrana gecisleri belirtmek icin cizilen kareler
    for (int z = 0; z < max_page; z++)
    {
        byte x = slider_page_property[z][0], y = slider_page_property[z][1];
        if (z == current_page - 1)
            display.fillRect(x, y, dot_width_height, dot_width_height, WHITE);
        else
            display.drawRect(x, y, dot_width_height, dot_width_height, WHITE);
    }
    display.display();
}
/* -------------------- GENEL ISLEMLER BITIS --------------------*/

/* -------------------- DERECE ISLEMLER BASLANGIC --------------------*/

String str_degree(signed char number)
{
    // Derecenin ekrana yazdırılması için string olarak dondurulmesi
    return String(number) + "C";
}

void increase_degree_calc_fan(){
    fan_level_last = fan_level_current;
    fan_level_current = calc_fan_level();

    if (fan_level_current != fan_level_last)
    {
        // Suanki fan leveli degistiyse
        signed char diff = fan_level_last - fan_level_current;
        if (diff > 0 && current_page == 1)
        {
            //
            for (char i = fan_level_last; i > fan_level_current; i--)
            {
                main_draw_fan(i, BLACK);
            }
        }
        fan_level_last = fan_level_current;
    }
}

void increase_degree()
{
    // Dereceyi arttırmak
    if (wanted_degree < degree_min_max[1])
    {
        wanted_degree++;

        increase_degree_calc_fan();

        update_write(wanted_degree_loc[0], wanted_degree_loc[1], str_degree(wanted_degree - 1), str_degree(wanted_degree));
        display.display();
    }
}
void decrease_degree_calc_fan(){
    fan_level_last = fan_level_current;
    fan_level_current = calc_fan_level();
    if (fan_level_current != fan_level_last)
    {
        // Suanki fan leveli degistiyse
        signed char diff = fan_level_current - fan_level_last;
        if (diff > 0 && current_page == 1)
        {
            for (char i = fan_level_current; i > fan_level_current - diff; i--)
            {
                main_draw_fan(i, WHITE);
            }
        }
        fan_level_last = fan_level_current;
    }
}
void decrease_degree()
{
    // Dereceyi dusurmek
    if (wanted_degree > degree_min_max[0])
    {
        wanted_degree--;
        decrease_degree_calc_fan();

        update_write(wanted_degree_loc[0], wanted_degree_loc[1], str_degree(wanted_degree + 1), str_degree(wanted_degree));
        display.display();
    }
}

void set_current_degree(signed char degree){
    if(current_degree_now != degree){
        current_degree_now = degree;
        if (current_page == 1)
            update_write(current_degree_loc[0], current_degree_loc[1], str_degree(current_degree_last), str_degree(current_degree_now));
        if (current_degree_now > current_degree_last)
        {
            decrease_degree_calc_fan();
        }else {
            increase_degree_calc_fan();
        }
        current_degree_last = degree;
    }
    
}
/* -------------------- DERECE ISLEMLER BITIS --------------------*/

/* -------------------- FAN ISLEMLER BASLANGIC --------------------*/

void main_draw_fan(byte level, unsigned char color)
{
    /*
    @params: 
        level   => Maximum fan seviyesi
        width   => Belirlediğiniz bir sayıya göre çizgi genişiliğini katsayılar
        height  => Belirlediğiniz fan grafiğinin uzunluğu
    */
    byte max_level = 4;
    byte width = 4 * level;
    byte height = 5;
    byte y = SCREEN_HEIGHT - height * level;
    if (level > 1)
        y -= (level - 1) * 2;

    display.fillRect(0, y, width, height, color);
    display.drawBitmap(width, y, main_fan_level, 5, 5, color);
}

void increase_fan()
{
    // Fanı arttırmak
    // Anasayfa ve fan sayfası icin
    if (fan_level_default < fan_min_max[1])
    {
        if(current_page == 2){
            add_fan();
            display.display();
        }
        fan_level_default++;
    }
}

void decrease_fan()
{
    // Fanı dusurmek
    // Anasayfa ve fan sayfası icin
    if (fan_level_default > fan_min_max[0])
    {
        if (current_page == 2){
            remove_fan();
            display.display();
        }
            
        fan_level_default--;
        
    }
}
byte calc_fan_level()
{
    // Suanki ve istenenen dereceye gore fan seviyesi belirlenmesi
    signed char diff = current_degree_now - wanted_degree;

    if (diff < 0)
        return 0;
    else if (diff >= 0 && diff < 1)
        return 1;
    else
        return fan_level_default;

}

void main_draw_fan_first(){
    // Sayfa ilk olusturuldugunda fan ikonlarının cizilmesi
    for (byte i = 1; i < fan_level_current+1; i++)
    {
        main_draw_fan(i,WHITE);
    }
}

void init_fan()
{
    // Fan sayfasına fanların ilk cizilisi
    if (fan_level_default > 0)
    {
        if (fan_level_default >= 1)
            draw_top_fan(WHITE);
        if (fan_level_default >= 2)
            draw_right_fan(WHITE);
        if (fan_level_default >= 3)
            draw_bottom_fan(WHITE);
        if (fan_level_default >= 4)
            draw_left_fan(WHITE);
    }
    draw_fan_circle(WHITE);
    write_normal(52, 5, WHITE, "FAN SEVIYESI");
    display.fillTriangle(77, 19, 70, 26, 84, 26, WHITE);
    display.fillTriangle(97, 26, 90, 19, 104, 19, WHITE);
    display.display();
}

void add_fan()
{
    // Tek bir fan silmek icin
    if (fan_level_default >= 3)
        draw_left_fan(WHITE);
    if (fan_level_default >= 2)
        draw_bottom_fan(WHITE);
    if (fan_level_default >= 1)
        draw_right_fan(WHITE);
}

void remove_fan()
{
    // Tek bir fan silmek icin
    if (fan_level_default <= 4)
        draw_left_fan(BLACK);
    if (fan_level_default <= 3)
        draw_bottom_fan(BLACK);
    if (fan_level_default <= 2)
        draw_right_fan(BLACK);
}

void draw_top_fan(unsigned char color)
{
    // 1 Numaralı fan
    display.drawBitmap(19, 1, top_fan, 10, 10, color);
}

void draw_right_fan(unsigned char color)
{
    // 2 Numaralı fan
    display.drawBitmap(28, 12, right_fan, 12, 12, color);
}
void draw_left_fan(unsigned char color)
{
    // 4 Numaralı fan
    display.drawBitmap(10, 8, left_fan, 10, 12, color);
}

void draw_bottom_fan(unsigned char color)
{
    // 3 Numaralı fan
    display.drawBitmap(16, 21, bottom_fan, 10, 10, color);
}

void draw_fan_circle(unsigned char color)
{
    // Fan ekranındaki nokta
    display.fillCircle(23, 16, 2, color);
}

/* -------------------- FAN ISLEMLER BITIS --------------------*/

/* -------------------- SAYFA CIZIM ISLEMLERI BASLANGIC --------------------*/
void page_boot_draw()
{
    // 0 Numaralı Sayfa
    current_page = 0;
    display.clearDisplay();
    display.display();

    display.drawBitmap(55, 0, on_off_image, 30, 32, WHITE);
    display.display();
}

void page_main_draw()
{
    // 1 Numaralı sayfa
    current_page = 1;
    display.clearDisplay();
    display.display();
    fan_level_current = calc_fan_level();
    main_draw_fan_first();
    write_normal(31, 4, WHITE, "ISTENEN");
    write_normal(wanted_degree_loc[0], wanted_degree_loc[1], WHITE, str_degree(wanted_degree));

    write_normal(81, 4, WHITE, "ICERISI");
    write_normal(current_degree_loc[0], current_degree_loc[1], WHITE, str_degree(current_degree_now));

    draw_main_lines();
    draw_all_dot();
    display.display();
}

void page_fan_draw()
{
    // 2 Numaralı sayfa
    current_page = 2;
    display.clearDisplay();
    display.display();

    init_fan();
    draw_all_dot();
}

void page_exit_draw()
{
    // 3 Numaralı sayfa
    current_page = 3;
    display.clearDisplay();
    display.display();
    draw_all_dot();
    display.drawBitmap(5, 3, danger_symbol, 5, 29, WHITE);
    write_normal(22, 6, WHITE, "KAPATMAK ICIN");
    write_normal(22, 18, WHITE, "ALTA BASINIZ");
    display.fillTriangle(116, 21, 106, 11, 128, 11, WHITE);
    display.display();
}

/* -------------------- SAYFA CIZIM ISLEMLERI BITIS --------------------*/

/* -------------------- SAYFA ISLEMLERI BASLANGIC --------------------*/

void next_page()
{
    // Sonraki ekrana gecis
    if (current_page+1 > max_page)
        current_page = 1;
    else
        current_page++;

    if (current_page == 1)
        page_main_draw();
    if (current_page == 2)
        page_fan_draw();
    if (current_page == 3)
        page_exit_draw();
}

void prev_page()
{
    // Onceki ekrana gecis yapmak icin
    if (current_page - 1 <= 0)
        current_page = max_page;
    else
        current_page--;

    if (current_page == 1)
        page_main_draw();
    else if (current_page == 2)
        page_fan_draw();
    else if (current_page == 3)
        page_exit_draw();
}

/* -------------------- SAYFA ISLEMLERI BITIS --------------------*/