% Email: enrico.leka@studenti.unipd.it

clear;      
fileDaLeggere = fopen('UTF8.data', 'r');    % Apertura del file in lettura contenente i dati in UTF8
fileInCuiScrivere=fopen('output.data','w');        % Apertura del file in scrittura, dove scriverò la decodifica
while ~feof(fileDaLeggere)              % Itero fino a che non arrivo all'End of File di UTF8.data
    A=fread(fileDaLeggere, 1);           % Leggo il byte guida, cioè quello che che contiene tanti 1 all'inizio in base a quanti byte che rappresentano il valore
    if isempty(A)                       % Se A è vuoto, mi fermo 
        break
    end                             
    primoByte = dec2bin(A, 8);     % Converto il byte letto in una stringa di 0 e 1 della lunghezza di 8 bit 
    i = 1;
    contaUno = 0;                     
    while(primoByte(i) ~= '0')        % Ciclo while per individuare il range dell'UTF8 in cui mi trovo in base al byte guida
        contaUno = contaUno + 1;
        i = i + 1;                      
    end                                                                                                                      
    if(contaUno == 7)                                       % Caso particolare (32 bit) in cui il primo byte non contiene nessun bit di informazione, quindi lo scarto, il secondo byte
        A=fread(fileDaLeggere, 1);                          % conterrà 2 bit di informazione, perchè i rimanenti 6 byte hanno 2 bit all'inizio (10) di codifica e 6 di informazione
        secondoByte = dec2bin(A, 8);                        % per un totale di 6*6=36 bit di informazione utile. Io devo codificarne 32, quindi il secondo byte conterra i primi due
        stringaDecodificata = extractBetween(secondoByte, 7, 8);    % bit di codifica (10), 36-32=4 bit mezzi a 0 forzatamente (10 0000 xx), quindi prendo solo i bit 7 e 8, come detto prima, due bit di informazione
        contaUno = contaUno - 1;
    else                        % Altrimenti mi trovo in uno dei casi della codifica UTF8 in cui, nel byte guida ci sono bit di informazione utile      
        stringaDecodificata=extractBetween(primoByte, contaUno+2, 8);    % I bit di informazione utile si trovano dopo i bit di controllo, quindi da (contaUno+2) fino all'ultimo (ottavo)
    end                                                         
    k=2;                                    
    while (k <= contaUno)                   % Qui andiamo a leggere i byte successivi in base al contaUno, cioè in base al caso di UTF8 in cui
        A = fread(fileDaLeggere, 1);        % ci troviamo estraendo solo i bit con informazione utile
        byte = dec2bin(A,8);                % Ogni volta estraggo un byte e prendo solamente gli ultimi 6 bit, senza quelli di codifica iniziali (10)
        stringaDecodificata = strcat(stringaDecodificata,extractBetween(byte, 3, 8));
        k = k + 1;
    end
    zeriIniziali = 32 - strlength(stringaDecodificata);         % Calcolo il numero di zeri da inserire in testa ai 4 byte per completarlli ed arrivare a 32 bit 
    for k = 1 : zeriIniziali            % Concaten zeri in testa 
        stringaDecodificata=strcat("0",stringaDecodificata);     
    end
    k=3;                            
    while k >= 0                  % Qui suddivido byte per byte partendo dall'ultimo perchè meno significativo, lo trasformo in decimale                                  
        fwrite(fileInCuiScrivere, bin2dec(extractBetween(stringaDecodificata, (k * 8) + 1, (k * 8) + 8)));       % (prima era una stringa) e scrivo sul file ogni byte in decimale  
        k = k - 1;
    end
end
% Chiudo i file 
fclose(fileDaLeggere); 
fclose(fileInCuiScrivere);

% COMMENTI
% -: Il file ha il nome sbagliato
% -: Nel calcolo del numero di uni iniziali vengono usate due variabili per fare lo stesso lavoro (i e contaUno)
% -: Usa un while al posto di un for (27)
% ~: Alcuni commenti di troppo, lo sanno tutti che fclose serve a chiudere i file, non serve specificarlo - 43
% -: Formattazione usata a tratti si a tratti no
% +: I file input.data ed output.data non differiscono
% Voto: /10