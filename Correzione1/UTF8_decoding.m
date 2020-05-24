%elena.scarpa.4@studenti.unipd.it
%%Decodifica UTF8
fd=fopen('UTF8.data','r'); %Apro il file UTF8.data che devo decodificare
w=fopen('output.data','w'); %Apro il file output.data in cui salverò la decodifica
r=fread(fd,'uint8'); %Procedo con la lettura di un byte alla volta
array=0;%Salvo in un array tutti i byte da decodificare
index=1;
while(index<=length(r))
    bin = dec2bin(r(index));
    bin=cast(bin,'uint8')-48; %Faccio il cast del numero binario senza segno
    j=1;
    fine=0;
    while(fine==0 && bin(j)==1) %Conto il numero di 1 
        if(j==length(bin))
            fine=1;
        end
        j=j+1;
    end
    number_ones=j-1; %Tolgo lo zero che ho contato (dato che ho inizializzato j a 1)
    if number_ones==0 && r(index)<128 %ASCII
        bin=dec2bin(r(index));
        bin=cast(bin,'uint8')-48;
        array=cat(2,array,bin);
        index=index+1;
    else       
        array=0;
        for k=index:index+number_ones-1 %number_ones non include lo 0
            bin=dec2bin(r(k));
            bin=cast(bin,'uint8')-48;             
            if k==index%siamo nel primo byte 
                a=bin(number_ones+2:length(bin));
                array=cat(2,array,a);
            else %siamo nei byte successivi
                if length(bin)>=3
                    a=bin(3:length(bin));
                    array=cat(2,array,a);
                end 
            end
        end 
    end
    if length(array)<8
        zeros=8-length(array);
        zeros=(1:zeros)*0;
        array=cat(2,zeros,array);
    end
    if length(array)>=8 && length(array)<=16
        zeros=16-length(array);
        zeros=(1:zeros)*0;
        array=cat(2,zeros,array);
    end
    if length(array)>=16 && length(array)<=24
        zeros=24-length(array);
        zeros=(1:zeros)*0;
        array=cat(2,zeros,array);
    end
    if length(array)>=24 && length(array)<32
        zeros=32-length(array);
        zeros=(1:zeros)*0;
        array=cat(2,zeros,array);
    end
    index=index+number_ones;
    dec=0;
    j=length(array)-7;
    while not(j==0)
        dec=2^(7)*array(j)+2^(6)*array(j+1)+2^(5)*array(j+2)+2^(4)*array(j+3)+2^(3)*array(j+4)+2^(2)*array(j+5)+2^(1)*array(j+6)+2^(0)*array(j+7);
        fwrite(w,dec,'uint8');
        j=j-8; %al bit del byte precedente
        if j<=0 %Condizione per uscire dal ciclo while
            j=0;
        end
    end
end
fclose('all');

% GRAVE: i file input.data ed output.data non combaciano
% 