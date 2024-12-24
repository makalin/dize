## Dize Programlama Dili ve Derleyicisi
Dize Programming Language & Compiler

Türkçe Tokenizer ve Kod Yorumlayıcı

```txt
  __ \  _) __  /  ____| 
  |   |  |    /   __|   
  |   |  |   /    |     
 ____/  _| ____| _____| 
```                    
                        
### Giriş

Bu proje, Türkçe programlama diline özgü anahtar kelimeler ve veri tipleri ile geliştirilmiş bir tokenizer ve kod yorumlayıcıdır. Türkçe karakter desteği, hata mesajları ve program yürütme gibi özellikler sunar.

### Özellikler

- **Türkçe Anahtar Kelimeler**: `eğer`, `değilse`, `döngü`, `yazdır`, `ve`, `veya`, `dizi` gibi Türkçe terimlerin kullanımı desteklenmektedir.
- **Türkçe Hata Mesajları**: Sözdizimi hataları, tür hataları, değişken hataları gibi durumlarda Türkçe hata mesajları görüntülenir.
- **Türkçe Karakter Desteği**: UTF-8 formatında Türkçe karakterleri doğru şekilde tanımlayabilen ve işleyebilen bir sistem.
- **Çoklu Veri Tipi Desteği**: Tam sayı, ondalık, metin, mantık ve dizi veri türlerini destekler.
- **Esnek Değişken Yönetimi**: Değişkenlerin kapsam yönetimi ve sabit değer atanması özellikleri bulunmaktadır.

### Dosya Yapısı

- **Ana Dosya**: `dc_v2.c`
- **Fonksiyonlar**:
  - `normalize_turkish_string`: Türkçe karakterleri normalize eder.
  - `error`: Türkçe hata mesajlarını oluşturur ve gösterir.
  - `get_next_token`: Kodun bir sonraki tokenını analiz eder.
  - `execute_program`: Programın ana yürütme işlevi.

### Kullanım

Kodunuzu bir dosyaya yazın ve yorumlayıcıya parametre olarak aktarın. Örnek bir dosya:

```txt
değişken x = 5;
eğer (x > 3) ise {
    yazdır "Merhaba Dünya!";
}
```

1. **Derleyiciler**: `dc` ve `dc_v2` dosyalarını derlemek ve çalıştırmak için `gcc` veya benzeri bir derleyici kullanabilirsiniz:
   ```bash
   gcc -o derleyici dc_v2.c
   ./derleyici program.dize
   ```

2. **Kod Örnekleri**: `.dize` uzantılı dosyalar, Türkçe anahtar kelimeleri destekleyen derleyicilerde çalıştırılmak üzere tasarlanmıştır.


### Dosyalar

#### **1. Gelişmiş Türkçe Dili Destekleyen Derleyici (dc_v2.c)**
Bu dosya, Türkçe anahtar kelimeler, hata mesajları ve veri türleri kullanarak bir derleyici oluşturmayı amaçlar. İçerdiği özellikler:
- Türkçe karakter desteği (`ç`, `ğ`, `ş` vb.).
- Türkçe anahtar kelimeler (`yazdır`, `eğer`, `döngü` gibi).
- Türkçe hata mesajları (ör. "Sözdizimi Hatası").
- Gelişmiş veri türleri ve değişken yönetimi.
- Türkçe'yi doğal bir programlama dili olarak kullanma amacı güden fonksiyonlar.

---

#### **2. Temel Türkçe Derleyici (dc.c)**
Bu dosya, daha temel bir Türkçe derleyici sunar. Öne çıkan özellikleri:
- Basit tokenizasyon ve sözdizimi analizi.
- Türkçe kontrol akış yapıları (`eğer`, `döngü`).
- Yazdırma ve değişken atama işlemleri.
- Basit hata yönetimi.

---

#### **3. Öğrenci Not Hesaplama Programı (not_hesapla.dize)**
Bu örnek program, öğrencinin vize ve final notlarını alarak başarı notunu hesaplar. Ayrıca devamsızlık kontrolü ve harf notu değerlendirmesi de içerir. Örnek:
```text
eğer başarı_notu >= 90
    yazdır "Harf Notu: AA"
değilse
    yazdır "Harf Notu: BB"
bitir
```

---

#### **4. Basit Sayaç Programı (sayac.dize)**
Bir döngü yapısı kullanarak 1'den 10'a kadar bir sayaç oluşturur. Sayaç yarıya geldiğinde özel bir mesaj yazdırır. Örnek:
```text
döngü sayı < hedef
    yazdır sayi
    sayı = sayı + 1
bitir
```

---

### Katkıda Bulunma
Bu projeye katkıda bulunmak için:
1. Yeni Türkçe programlama örnekleri ekleyebilirsiniz.
2. Mevcut derleyicileri geliştirmek için kod gönderebilirsiniz.
3. Hata veya eksik bildirimlerinde bulunabilirsiniz.

---

### Lisans
Bu proje, açık kaynaklıdır ve herhangi bir şekilde kullanılabilir. Ancak, katkıda bulunanların isimlerinin belirtilmesi rica olunur.
