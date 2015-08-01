/***************************************************************
*     Hızlı Fourier Dönüşümü:                                  	*
*                                                              	*
*       N-1                                                    	*
*       ___       -j(2*(PI)/N)*k*n                            	*
*   X = \   x[n] e                                            	*
*    k  /__                                                   	*
*       n=0                                                    	*
*                                                              	*
*    bağıntısının hızlı hesaplanan halidir.                    	*
*                                                               *
****************************************************************/
/***************************************************************
*                                                               *
*       Yazar            : Cerem Cem ASLAN                      *
*                                                               *
*       Başlama tarihi   : Ne bileyim ben (oldu bayaa)          *
*       Bitiş tarihi     : 22 Haziran Cuma, 2007 saat: 03.43 GMT+2   *
*                                                               *
*       AÇIKLAMA         :                                      *
*                                                               *
*       Dur şimdi, birazdan açıklarım                           *
*                                                               *
*       YAPILACAKLAR     :                                      *
*                                                               *
*       * Dosya okuma/yazma yaparken şu anki dizini daha        *
*         kesin bir yolla buldur.                               *
*                                                               *
*       * Dosyadan okuma yapılacağı zaman ilgili dosyanın       *
*         yerini program parametresi olarak girdir.             *
*                                                               *
*                                                               *
*                                                               *
*                                                               *
****************************************************************/
/// Önişlemci tanımlamaları:
#define HFD_KULLAN ; // Dönüşümü Hızlı Fourier Dönüşüm ile yapacaksa bunu tanımla, tanımlamazsan AFD ile yapar.
//#define EKRANA_YAZDIR ; // Ekrana çıktı al
#define DOSYAYA_YAZDIR ; // Tanımlandıysa Fourier dönüşümü alınmış noktaları dosyaya yazdır
#define BILGILENDIR ; // Tüm bilgilendirme çıkışlarını açar ya da kapatır

#include <iostream>		// Standart giriş çıkış için
#include <iomanip>		// Çıkan rakamları biçimlendirmek için
#include <math.h>		// Matematik işlemleri için
#include <fstream>		// Dosyadan okumak ve dosyaya yazmak için
#include <vector>		// Noktaların tutulacağı vektör diziler için

// İsim uzayları
using namespace std;

//Sık kullanılacak uzun isimli değişken türlerine takma isimler ver
typedef vector<double> 			Vektor;
typedef vector<double>::iterator	VektorGostericisi;

// Karmaşık değerler içerecek listeler için yapı tanımla
struct Karmasik {double gercel, sanal, mutlak;};
struct KarmasikVektor {Vektor gercel, sanal, mutlak;};
struct VektorIndis {Vektor tumu, tek, cift;};
struct Kuvvet{unsigned long int sonuc, us;};
struct KarmasikDosya {ofstream gercel, sanal, mutlak;};


class VeriAl
{
	// Alınan veriden tek indisli olanlar ile çift indisli olanları ayırmak için
	// hazırlanmış sınıftır.
	private:

	public:
		Vektor tumGirdi; // alınan tüm değerler buraya aktarılacak
		Vektor tekIndisliler, ciftIndisliler; // YAPILACAK : DIŞARIDAN BUNLARI DEĞİŞTİRMEYİ ENGELLE (ERİŞİMİ ENGELLE_ME_)

		VeriAl(Vektor&); //  Yüklenici (başlangıç değeri var)
		VeriAl(); // Yüklenici (başlangıç değeri yok) (aşırı yükleme yapıyoruz)

		void Ayikla(); // Nesne ilklendirilmemişse bu işlevle tek ve çift olanları ayıkla
		void HamVeriAl(Vektor&);
};
VeriAl::VeriAl(Vektor& girdi)
{
	// Başlangıç değeri varsa girilen değer tüm değerleri içeren bir vektör olmalı,
	HamVeriAl(girdi);
}
VeriAl::VeriAl()
{
	// Başlangıç değeri yoksa telaşlanma, zamanla o da olur...
}
void VeriAl::Ayikla()
{
	// tümGirdi içinden tek indisli olanları ayıklayıp tekIndisliler'e aktar
	// tümGirdi içinden çift indisli olanları ayıklayıp ciftIndisliler'e aktar

	// Tek indisli dizi elemanlarını ayrı bir veri kabına aktar
	for (unsigned int i = 1; i < tumGirdi.size(); i += 2)
	{
		tekIndisliler.push_back(tumGirdi[i]);
	}

	// Çift indisli elemanları ayıklayıp ayrı bir veri kabına aktar
	for (unsigned int i = 0; i < tumGirdi.size(); i += 2)
	{
		ciftIndisliler.push_back(tumGirdi[i]);
	}
}
void VeriAl::HamVeriAl(Vektor& girdi)
{
	tumGirdi = girdi;
	Ayikla();
}

// Kullanılan işlevleri bildir
KarmasikVektor AFD(Vektor&);
KarmasikVektor HFD(Vektor&);
double Mutlak (double, double); // İkide bir mutlak değer mi hesaplayacaz arkadaş... nedir yani...
Kuvvet IkininKuvveti(unsigned long int);
Vektor DosyadanOku(void);
Vektor KlavyedenGiris(void);
void ProgramHakkinda(void);

int main(int argc, char** argv)
{
	ProgramHakkinda();

	long double baslamaZamani = clock();

	/// VERİ GİRİŞİ YAPILIYOR ///
	bool klavyedenGirisYap = false; // "hayır" diyorsan dosyadan giriş yapacaksın demektir
	Vektor girilenVeri = (klavyedenGirisYap) ? KlavyedenGiris() : DosyadanOku();

	if(klavyedenGirisYap)
	{
		// Klavyeden giriş yaptırılıyorsa kullanıcıya ne girdiğini göster
		cout << "[i] \t xZaman.tumGirdi[i] \t xZaman.tekIndisliler[i] \t xZaman.ciftIndisliler[i]" << endl;
		cout << "------------------------------------------------------------------------------" << endl;
		for (unsigned int i=0; i < girilenVeri.size(); i++)
		{
			cout << "[ " << i << " ] \t ";
			cout << girilenVeri[i] << " \t ";
			if (i%2 == 0)
			{
				// Şu anda çift indisteyiz
				cout << " ----- " << " \t "; // Tek indisli sayı yerine boşluk göster
				cout << " \t " << girilenVeri[i/2]; // Çift indisli sayıyı göster
			}
			else
			{
				// Bir sayı çift değilse tektir. Genelde böyle yani...
				// Yani şu an tek indisteyiz
				cout << girilenVeri[i/2] << " \t " ; //  Tek indisli sayıyı göster.
				cout << " \t " << " ----- "; // çift indisli sayı yerine boşluk göster
			}
			cout << endl; // bir satır aşağı in
		}
	}
	// Fourier dönüşümünü hesapla, vektör liste'ye aktar
	KarmasikVektor xFrekans;
#ifdef HFD_KULLAN
	/**
	* HFD() işlevi, girdi adedinin N = 2^a şeklinde olduğunu kabul ederek işlem yapar.
	* Dolayısıyla eğer girdi adedi 2^a'dan farklı bir sayıdaysa, girdiler 2'nin bir
	* kuvveti ile ifade edilebilene kadar sonuna sıfırlar ekle.
	*/
	cout << "Toplam ornek sayisi: " << girilenVeri.size() << endl;
	Kuvvet enFazlaNoktaSayisi = IkininKuvveti(girilenVeri.size());
	if (enFazlaNoktaSayisi.sonuc < girilenVeri.size())
	{
		// ORNEK_SAYISI 2^a şeklinde değil, sonuna 0'lar ekleyerek o hale getir
		/**
		* Zaman uzayındaki işaretin sonuna sıfırlar eklendiğinde bu işaretin
		* frekans uzayındaki karşılığı, özgün haldeki karşılığının ara değerleri bulunmuş
		* halini verir.
		*
		* Yalnızca işlemleri daha hızlı yapmak için başvursak da bu işlemin sonucunda
		* elde edilen frekans spektrumu özgün halden daha yüksek çözünürlüğe sahip
		* olacaktır.
		*
		* Dikkat edilmesi gereken nokta şudur ki, elde edilen X[k]'ların sayısı, artık
		* örnek sayısına eşit değildir. Yeni X[k]'lar frekans ekseninde tekrar
		* ölçeklendirildikten sonra kullanılmalıdır.
		*/
		unsigned long int girilmesiGerekenSifirMiktari = (enFazlaNoktaSayisi.sonuc * 2) - girilenVeri.size();
		cout << "Buna en yakin ve 2^a kivamindaki bir ust sayi: ";
		cout << enFazlaNoktaSayisi.sonuc * 2 << ", dolayisiyla bu noktalarin sonuna ";
		cout << girilmesiGerekenSifirMiktari << " tane 0 eklemeliyim \n";

		girilenVeri.insert(girilenVeri.end(), girilmesiGerekenSifirMiktari, 0);

		cout << "su anda girilen verinin son eleman sayisi: " << girilenVeri.size() ;
		enFazlaNoktaSayisi = IkininKuvveti(girilenVeri.size());
		cout << "(tam olarak 2^" << enFazlaNoktaSayisi.us << "\'e tekabul ediyor" << endl;
	}

	VeriAl xZaman(girilenVeri);
	xFrekans = HFD(xZaman.tumGirdi);

#else//AFD kullan o zaman
	VeriAl xZaman(girilenVeri);
	xFrekans = AFD(xZaman.tumGirdi);
#endif//HFD_KULLAN


#ifdef EKRANA_YAZDIR
	// Hesaplanan fourier dönüşümünü bir zahmet ekrana yazdır
	cout << endl << "frekans listesinin eleman sayisi: " << xFrekans.mutlak.size() << endl;
	cout << "[i] \t xFrekans.gercel[i] \t xFrekans.sanal[i] \t xFrekans.mutlak[i]" << endl;
	cout << "------------------------------------------------------------------------------" << endl;

	for (unsigned int i = 0; i < xFrekans.mutlak.size(); i++)
	{
		cout << scientific << "[ " << i << " ] \t " << xFrekans.gercel[i] << " \t " << xFrekans.sanal[i] << " \t " << xFrekans.mutlak[i] << " \t " << endl;
	}
#endif//EKRANA_YAZDIR

#ifdef DOSYAYA_YAZDIR

	KarmasikDosya xFrekansDosyasi;

	// Gerçel kısımları dosyaya yaz
	xFrekansDosyasi.gercel.open("./Fourier/xFrekans.Gercel.CeremCem.txt", ios::out | ios::trunc);
	if (!xFrekansDosyasi.gercel)
	{
		cerr << "Cikis dosyasi (Gercel) olusturulamadi";
		return 1;
	}
	else
	{
		cout << endl << endl;
		cout << "xFrekans.gercel dosyaya yaziliyor..." << endl;
		for (unsigned int i=0; i < xFrekans.gercel.size(); i++)
		{
			xFrekansDosyasi.gercel << scientific << xFrekans.gercel[i] << endl;
		}
		xFrekansDosyasi.gercel.close();
	}

	// Sanal kısımları dosyaya yaz
	xFrekansDosyasi.sanal.open("./Fourier/xFrekans.Sanal.CeremCem.txt", ios::out | ios::trunc);
	if (!xFrekansDosyasi.sanal)
	{
		cerr << "Cikis dosyasi (Sanal) olusturulamadi";
		return 1;
	}
	else
	{
		cout << "xFrekans.sanal dosyaya yaziliyor..." << endl;
		for (unsigned int i=0; i < xFrekans.sanal.size(); i++)
		{
			xFrekansDosyasi.sanal << scientific << xFrekans.sanal[i] << endl;
		}
		xFrekansDosyasi.sanal.close();
	}
	xFrekansDosyasi.mutlak.open("./Fourier/xFrekans.Mutlak.CeremCem.txt", ios::out | ios::trunc);
	if (!xFrekansDosyasi.mutlak)
	{
		cerr << "Cikis dosyasi (Mutlak) olusturulamadi";
		return 1;
	}
	else
	{
		cout << "xFrekans.mutlak dosyaya yaziliyor..." << endl;
		for (unsigned int i=0; i < xFrekans.mutlak.size(); i++)
		{
			xFrekansDosyasi.mutlak << scientific << xFrekans.mutlak[i] << endl;
		}
		xFrekansDosyasi.mutlak.close();
	}

	cout << "Tamam, hepsini yazdim. Hadi git selametle." << endl;
#endif//DOSYAYA_YAZDIR
#ifdef BILGILENDIR
	cout << endl << endl;
	cout << "Bu kodun isletme suresi: " << (clock() - baslamaZamani) / CLOCKS_PER_SEC << " saniye" << endl;
#endif//BILGILENDIR

	return 0; // Programı hatasız sonlandır
}


/***************************************************************
*                                                              *
* Programda kullanılan işlevler buradan sonra tanımlanacaktır. *
*                                                              *
***************************************************************/


KarmasikVektor
AFD(Vektor& donusturulecekNoktalar)
{
	/**
	* Vektor türündeki zaman noktalarının yer göstericisini kullanıp
	* işlemden sonra Karmaşık türündeki veriyi döndüren işlev
	*/
	// Zaman uzayında kaç tane nokta olduğunu bul
	unsigned const int ORNEK_SAYISI = donusturulecekNoktalar.size();

#ifndef HFD_KULLAN
	// İşlevin devreye girdiğini kullanıcıya bildir:
	cout << endl;
	cout << "AFD() islevi devreye girdi, ";
	cout << ORNEK_SAYISI << " adet nokta uzerinde islem yapilacak" << endl;
#endif//HFD_KULLAN

	// Hesaplanan frekans uzayındaki değerlerin aktarılacağı liste(ler)
	KarmasikVektor frekanstakiNoktalar;

	// Ayrık Fourier Dönüşümü için hesaplama
	// Zaman uzayındaki nokta sayısı kadarlık döngü aç
	// k. frekans değerini hesapla
	for (unsigned int k = 0; k < ORNEK_SAYISI; k++)
	{
	#ifndef HFD_KULLAN
		// Örnek sayısının %ADIM_BUYUKLUGU kadarı hesaplandığında bunu bildir
		int yuzde = 100 * (k+1) / ORNEK_SAYISI;
		const int ADIM_BUYUKLUGU = 5;
		if ((yuzde % ADIM_BUYUKLUGU) == 0)
		{
			cout << " %" << yuzde << " tamamlandi..." << "\r";
		}
	#endif//HFD_KULLAN
		// Kullanılacak geçici değerleri ilklendir
		double geciciGercel=0.0;
		double geciciSanal=0.0;

		// Zamandaki noktalara ulaşmak için liste göstericisi tanımla ve onu ilklendir
		VektorGostericisi x = donusturulecekNoktalar.begin();

		// Toplam işlemi
		// n. zaman değerini işleme al
		for (double n = 0.0; n < ORNEK_SAYISI; n++)
		{
			// Trigonometrik fonksiyonun içini bu şekilde sadeleştir
			double p = 2.0 * M_PI / ORNEK_SAYISI * k * n;
	//*		// Gerçel ve sanal frekans değerlerini bul
			geciciGercel 	+= (*x) * cos(p);
			geciciSanal		+= (*x) * sin(p);
			x++;
	/*
			geciciGercel 	+= donusturulecekNoktalar[n] * cos(p);
			geciciSanal		+= donusturulecekNoktalar[n] * sin(p);
	*/
		}
		// Hesaplanan frekans değerlerinin gerçel ve sanal kısımlarını
		// ilgili dizilere aktar:
		frekanstakiNoktalar.gercel.push_back(geciciGercel);
		frekanstakiNoktalar.sanal.push_back(geciciSanal);

		/**
		* frekans = frekans.gercel + j frekans.sanal
		*                    ____________________________________
		* frekans.mutlak = \/ (frekans.gercel)² + (frekans.sanal)²
		*
		*/

		// Hesaplanan mutlak frekans değerini frekanstakiNoktalar.mutlak dizisine koy
		frekanstakiNoktalar.mutlak.push_back(Mutlak(geciciGercel, geciciSanal));
	}
	return frekanstakiNoktalar;
}

double
Mutlak (double x, double y)
{
	return sqrt( pow(x, 2) + pow(y, 2) );
}

KarmasikVektor
HFD(Vektor& noktaDizisi)
{
	VeriAl xZaman(noktaDizisi);
	// xZaman'nın tumGirdi, tekIndisliler ve ciftIndisliler olmak üzere 3 kısmı var.
	// Nokta sayısının 2^a şeklinde olduğunu varsayalım.

	KarmasikVektor xFrekans, xFrekansTek, xFrekansCift;

	if (xZaman.tumGirdi.size() == 2)
	{
		// Geriye yalnızca 1 nokta kalmışsa artık ayrık fourier dönüşümünü hesapla
		xFrekans = AFD(xZaman.tumGirdi);
	}
	else
	{
		// Hala daha küçük AFD'lere bölebiliniyor, bölelim o zaman...
		xFrekansTek = HFD(xZaman.tekIndisliler);
		xFrekansCift = HFD(xZaman.ciftIndisliler);


		/**
		* Gerekli matematiksel denkleme koyarak tek ve çift indisli noktaların
		* Fourier dönüşümlerini topla
		*                                               -j(2*PI/N)k
		* T_k'ları hesaplarken sabit olarak kullanılan e             ifadesini
		* döngüden önce düzenle
		*
		*  -j(2*PI/N)k
		* e            = cos ((2*PI/N)k) - j sin ((2*PI/N)k)
		*
		*/
		unsigned const int N = xZaman.tumGirdi.size();
		for (unsigned int k=0; k < N; k++)
		{
			Karmasik ustelIfade;
			double geciciXfrekansGercel, geciciXfrekansSanal;
			const double a = 2*M_PI/N;
			unsigned const int M = N/2;

			if (k < M)
			{
				ustelIfade.gercel = cos (a*k);
				ustelIfade.sanal = sin (a*k);
				geciciXfrekansGercel = xFrekansCift.gercel[k]
						+ ustelIfade.gercel * xFrekansTek.gercel[k] - ustelIfade.sanal * xFrekansTek.sanal[k];

				geciciXfrekansSanal = xFrekansCift.sanal[k]
						+ ustelIfade.gercel * xFrekansTek.sanal[k] + ustelIfade.sanal * xFrekansTek.gercel[k];

			}
			else// if (k >= M)
			{

				ustelIfade.gercel = cos (a*(k-M));
				ustelIfade.sanal = sin (a*(k-M));
				geciciXfrekansGercel = xFrekansCift.gercel[k-M]
						- ustelIfade.gercel * xFrekansTek.gercel[k-M] + ustelIfade.sanal * xFrekansTek.sanal[k-M];

				geciciXfrekansSanal = xFrekansCift.sanal[k-M]
						- ustelIfade.gercel * xFrekansTek.sanal[k-M] - ustelIfade.sanal * xFrekansTek.gercel[k-M];
			}

			xFrekans.gercel.push_back( geciciXfrekansGercel );
			xFrekans.sanal.push_back( geciciXfrekansSanal );
			xFrekans.mutlak.push_back( Mutlak(geciciXfrekansGercel, geciciXfrekansSanal));
		}
	}
	return xFrekans;
}

Kuvvet
IkininKuvveti(unsigned long int giris)
{
	// giris'e küçük eşit olup aynı zamanda ve 2'nin kuvveti olan en büyük sayıyı
	// geri döndürür
	Kuvvet cikis;
	unsigned long int gecici;
	cikis.us = 0;
	while (true)
	{
		gecici = (unsigned long int) pow (2, cikis.us);
		if (gecici > giris)
		{
			// sonuç olarak hesaplanan sayı girişi aşmışsa, demek ki bir önceki
			// i değeri, bu sayının en büyük 2^i değerini sağlayan i değeridir.
			// O, budur. Evet evet. Budur. Tanışmamış mıydınız daha önce? Bu o işte?
			cikis.us--;
			return cikis;
		}
		else
		{
			cikis.us++;
			cikis.sonuc = gecici;

		}
	}
}

Vektor
KlavyedenGiris(void)
{
	Vektor veriGirisi;
	// Klavyeden bilinmeyen sayıda zaman uzayında örnek girdir:
	cout 	<< "Lutfen orneklenmis isaretin noktalarini giriniz. "
			<<	"Giris islemi bittiginde \\0 giriniz. Bu, NULL anlamina gelir."
			<< "Bu karakter girildiginde giris yapilan donguden cikilacak ve programin akisina devam edilecektir."
			<< endl << endl;
	double 	gecici;
	int 	sayac=0;
	while (true)
	{
		cout << "{{ " << sayac+1 << ". ornek }} \t" << "x[" << sayac << "]=";
		sayac++;
		if (!bool (cin>>gecici))
		{
			// "\0" (yani NULL) karakteri girildiyse giriş tamamlanmıştır
			// Döngüden çık, gez dolaş, işine gücüne bak
			break;
		}
		veriGirisi.push_back(gecici);
	}
	return veriGirisi;
}

Vektor
DosyadanOku() /// YAP!!! OKUMA YAPILACAK DOSYA ADI PARAMETRİK OLARAK VERİLECEK
{
	Vektor veriGirisi;
	ifstream veriDosyasi; 	// Zaman uzayındaki noktaların okunacağı dosya
	// Zaman uzayındaki noktaları dosyadan oku
	veriDosyasi.open("./Fourier/xZaman.txt");
	// Okuyamazsan haberimiz olsun
	if (!veriDosyasi)
	{
		cerr << endl << "Zaman uzayindaki noktalarin bulundugu dosya okunamadi ya da bulunamadi.";
		cerr << endl
		<< "Lutfen noktalarin calisma dizininde (buyuk ihtimalle bu programin hemen yaninda) "
		<< "Fourier isimli dizin altinda xZaman.txt adinda oldugundan ve noktalarin alt alta "
		<< "yazildigindan emin olun." << endl << "Ileriki surumlerde parametrik giris ile "
		<< "bu sorun cozulecektir." << endl;
	}
	else
	{
		//cout << "Zaman uzayındaki noktalar dosyadan okunabiliyor. Süperim ben" << endl;
	}
	double nokta;
	while (veriDosyasi >> nokta)
	{
		veriGirisi.push_back(nokta);
	}
	veriDosyasi.close();

	return veriGirisi;
}

void
ProgramHakkinda()
{
	cout << endl;
#ifdef HFD_KULLAN

	cout << "Hizli Fourier Donusumunu hesaplayan programdir. ";
#else//AFD kullanılacak

	cout << "Ayrik Fourier Donusumunu hesaplayan programdir. ";
#endif//HFD_KULLAN
	cout << "(surum : 1.0)" << endl;
	cout << "Cerem Cem ASLAN, " << __DATE__ << endl << endl;
}
