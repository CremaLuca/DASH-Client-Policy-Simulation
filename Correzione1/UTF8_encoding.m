%elena.scarpa.4@studenti.unipd.it
%%Codifica UTF-8 in Big Endian
fd=fopen('input.data','r'); %Apro il file input.data
r=fread(fd,'uint32'); %Leggo il file input.data, 4 byte alla volta
w=fopen('UTF8.data','w'); %Apro il file in cui scrivero loutput
for g=1:length(r)
    b=dec2bin(r(g))-48; %Rappresentazione binaria di r(g)
    if(r(g)<=127) %Sono nel primo caso, in cui mi servono 7 bit
        fwrite(w,(r(g)));
    end
    if(r(g)>127 && r(g)<=2047) %Sono nel secondo caso, in cui mi servono 11 bit
        b=zeros(b,11); %Aggiungo gli zeri che mancano per avere 11 bit
        byte1=primoByte(b,2);
        byte2=Byte(b,6,11);
        fwrite(w,byte1); %Scrivo nel file di output il primo byte
        fwrite(w,byte2); %Scrivo nel file di output il secondo byte
    end
    if(r(g)>2047 && r(g)<=65535) %Sono nel terzo caso, in cui mi servono 16 bit
        b=zeros(b,16);
        byte1=primoByte(b,3);
        fwrite(w,byte1);
        byte2=Byte(b,5,10);
        fwrite(w,byte2);
        byte3=Byte(b,11,16);
        fwrite(w,byte3);
    end
    if(r(g)>65535 && r(g)<=2097151) %Sono nel quarto caso, in cui mi servono 21 bit
        b=zeros(b,21);
        byte1=primoByte(b,4);
        fwrite(w,byte1);
        byte2=Byte(b,4,9);
        fwrite(w,byte2);
        byte3=Byte(b,10,15);
        fwrite(w,byte3);
        byte4=Byte(b,16,21);
        fwrite(w,byte4);
    end
    if(r(g)>2097151 && r(g)<=67108863) %Sono nel quinto caso, in cui mi servono 26 bit
        b=zeros(b,26);
        byte1=primoByte(b,5);
        fwrite(w,byte1);
        byte2=Byte(b,3,8);
        fwrite(w,byte2);
        byte3=Byte(b,9,14);
        fwrite(w,byte3);
        byte4=Byte(b,15,20);
        fwrite(w,byte4);
        byte5=Byte(b,21,26);
        fwrite(w,byte5);
    end
    if(r(g)>67108863 && r(g)<=2147483647) %Sono nel sesto caso, in cui mi servono 31 bit
        b=zeros(b,31);
        byte1=primoByte(b,6);
        fwrite(w,byte1);
        byte2=Byte(b,2,7);
        fwrite(w,byte2);
        byte3=Byte(b,8,13);
        fwrite(w,byte3);
        byte4=Byte(b,14,19);
        fwrite(w,byte4);
        byte5=Byte(b,20,25);
        fwrite(w,byte5);
        byte6=Byte(b,26,31);
        fwrite(w,byte6);
    end
    if(r(g)>2147483647) %Sono nel caso in cui mi servono 32 bit
        b=zeros(b,36);
        byte1=primoByte(b,7);
        fwrite(w,byte1);
        byte2=Byte(b,1,6);
        fwrite(w,byte2);
        byte3=Byte(b,7,12);
        fwrite(w,byte3);
        byte4=Byte(b,13,18);
        fwrite(w,byte4);
        byte5=Byte(b,19,24);
        fwrite(w,byte5);
        byte6=Byte(b,25,30);
        fwrite(w,byte6);
        byte7=Byte(b,31,36);
        fwrite(w,byte7);
    end
end

function y=zeros(a,n) %funzione che aggiunge gli zeri
    z=n-length(a); %Conto quanti zeri devo aggiungere
    zeri=[1:z]*0; %Aggiungo gli zeri
    y=cat(2,zeri,a); %concateno gli zeri
end
function y=primoByte(b,n) %Scrivo il primo byte
    a=0;
    %Sommo le potenze di 2 da 2 alla 7 fino a 2 alla 7-numero byte
    for j=1:n
        a=a+2^(8-j);
    end
    j=j+2;
    i=1;
    for x=j:8
        a=a+2^(8-x)*b(i);
        i=i+1;
    end
    y=a;
 end
 
 function y=Byte(b,min,max) %Scrivo i byte successivi al primo
    a=1*2^7+0*2^6;
    for x=min:max
        a=a+2^(max-x)*b(x);
    end
    y=a;
 end
 
 % ~: Manca clear all'inizio del codice, non è un problema siccome tutte le
 % variabili sono inizializzate.
 % -: A cosa serve il "-48" nella conversione da decimale a binario? Non è
 % spigato da nessuna parte e non è per niente intuitivo.
 % -: Per ogni controllo va a leggere nel file, poteva usare una variabile
 % ausiliaria in cui salvare il valore di r(g)
 % -: Un metodo si chiama con la lettera maiuscola, gli altri due con la
 % lettera minuscola
 % -: 7 diversi if quando si poteva risolvere in due righe con un ciclo
 % -: documentazione del metodo "zero" non sufficientemente specifica, non
 % si capisce dove aggiunga gli zeri e a cosa - 85
 % -: Non si capisce e non è spiegato per quale motivo magico j=j+2 - 96
 % -: Documentazione dice "Scrivo" quando in realtà li costruisce e basta,
 % non scrive su file