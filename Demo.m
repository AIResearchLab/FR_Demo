try
   clear
catch ME
    disp(ME);
end

positionSwitchAmount = 350;
s = serialport("COM16",9600);
y = clock;
secondsNow = y(6); 
minutesNow = y(5);
minuteLast = minutesNow;
secondsLast = secondsNow;
elapsedWindow = 3;
flush(s);
while 1
    y = clock;
    secondsNow = y(6);  
    minutesNow = y(5);
    if(secondsNow>=secondsLast+elapsedWindow)
        positionSwitchAmount = positionSwitchAmount*-1;
        position = [2048 2048 2048 2048]+positionSwitchAmount;
        appendage = typecast(uint16(position(1)),'uint8');
        appendage = [appendage(2) appendage(1)];
        byteArray = [uint8(255) uint8(16)];
        byteArray = horzcat(byteArray,[uint8(23) uint8(40) appendage]);
        byteArray = horzcat(byteArray,[uint8(24) uint8(40) appendage]);
        byteArray = horzcat(byteArray,[uint8(25) uint8(40) appendage]);
        byteArray = horzcat(byteArray,[uint8(26) uint8(40) appendage]);
        byteArray = horzcat(byteArray,[uint8(0) uint8(0)]);    
        write(s,byteArray,'uint8');
        disp('writehere');
        minuteLast = minutesNow;
        secondsLast = secondsNow;
    end
    if(minuteLast<minutesNow)
        minuteLast = minutesNow;        
        secondsLast = secondsNow;
    end
    
    i = read(s,36,'uint8');
    recieveSpeedandPosition(i);
end

function recieveSpeedandPosition(x)
    pI = 5;
    positions = [   typecast(uint8([x(pI+1) x(pI)]), 'uint16'),typecast(uint8([x(pI+5) x(pI+4)]),'uint16'),...
typecast(uint8([x(pI+9) x(pI+8)]), 'uint16'),typecast(uint8([x(pI+13) x(pI+12)]), 'uint16')];
    disp(positions);
    speeds = [   typecast(uint8([x(pI+17) x(pI+16)]), 'uint16'),typecast(uint8([x(pI+21) x(pI+20)]),'uint16'),...
                 typecast(uint8([x(pI+25) x(pI+24)]), 'uint16'),typecast(uint8([x(pI+29) x(pI+28)]), 'uint16')]; 
    disp(speeds);
end