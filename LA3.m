clear

%Load values from file
filename = 'inputs.txt';
ifile = fopen(filename); 
inp.mass = fscanf(ifile, '%f', 1); %0.63
inp.drag = fscanf(ifile, '%f', 1); %0.0006
inp.v0= fscanf(ifile, '%d', 1); %22
inp.dscreen = fscanf(ifile, '%d', 1); %10
inp.hscreen = fscanf(ifile, '%d', 1); %10
inp.dbasket = fscanf(ifile, '%d', 1); %15
inp.dwall = fscanf(ifile, '%d', 1); %25
inp.vwind = fscanf(ifile, '%d', 1); %-10
inp.tolerance = fscanf(ifile, '%f', 1); %0.001

%Converging direct throw
subplot(2, 2, 1)
atheta = 0;
btheta = 90;
mid = (atheta + btheta)/ 2;
status.place = 0;
[xf, ~, ~, zmax, ~] = draw(status, mid, inp);
hold on
while (xf - inp.tolerance) > 15 || (xf+inp.tolerance) < 15
    if (xf - inp.tolerance) > 15 || abs(xf-inp.dscreen)< inp.tolerance
        atheta = mid;
        mid = (atheta + btheta)/ 2;
        [xf, ~, ~, zmax, ~] = draw(status, mid, inp);
    elseif (xf+inp.tolerance) < 15
         btheta = mid;
         mid = (atheta + btheta)/ 2;
         [xf, ~, ~, zmax, ~] = draw(status, mid, inp);
    end
end
direct_throw_angle = mid %Print angle for straight throw
direct_throw_maximum_height = zmax %Print max height for straight throw
style(inp);
title('Converging direct throws')
hold off 

%Plot last direct throw
subplot(2, 2, 2)
[~, ~, idzmax, zmax, ~] = draw(status, mid, inp);
style(inp);
title('Approximate direct throw')
plot(idzmax, zmax, 'ko') %Display max height
maxheight = char('Maximum height', num2str(zmax), 'm');
text(15, 20, maxheight, 'FontSize', 7) 
final_theta = char('Around', num2str(round(mid)), 'degrees'); 
text(3.5, 10, final_theta,'FontSize', 6) %Display approx final angle
hold off

%Converging trick shot
subplot(2, 2, 3)
atheta = 45;
btheta = 90;
mid = (atheta + btheta)/ 2;
[xf, zf, ~, ~, vf] = draw(status, mid, inp);
if round(xf)==25
    status = adjust(xf, zf, vf);
    hold on
    [xf, ~, ~, ~, ~] = draw(status, mid, inp);
end
while (xf - inp.tolerance) > 15 || (xf+inp.tolerance) < 15
    if (xf - inp.tolerance) > 15
        btheta = mid;
        mid = (atheta + btheta)/ 2;
        status.place = 0;
        [xf, zf, ~, ~, vf] = draw(status, mid, inp);
        if round(xf)==25
            status = adjust(xf, zf, vf);
            [xf, ~, ~, ~, ~] = draw(status, mid, inp);
        end
    elseif (xf+inp.tolerance) < 15
        atheta = mid;
        mid = (atheta + btheta)/ 2;
        status.place = 0;
        [xf, zf, ~, ~, vf] = draw(status, mid, inp);
        if round(xf)==25
            status = adjust(xf, zf, vf);
            [xf, ~, ~, ~, ~] = draw(status, mid, inp);
        end
    end
end
[~, ~, ~, ~, ~] = draw(status, mid, inp);
style(inp);
title('Converging trick shots');
hold off

%Plot last trick shot
subplot(2, 2, 4)
status.place = 0;
[xf, zf, ~, ~, vf] = draw(status, mid, inp);
hold on
plot(inp.dwall, zf, 'ko');
hit = char('Hit Location: ', num2str(zf), 'm');
trick_shot_hit_location = zf
text(18, 13, hit, 'FontSize', 7); %Display hit location
status = adjust(xf,zf,vf);
[xf, zf, maxindex, zmax, vf] = draw(status, mid, inp);
style(inp);
final_theta = char('Around', num2str(round(mid)), 'degrees'); 
text(4, 5, final_theta,'FontSize', 6) %Display approx final angle
title('Approximate trick shot');
hold off
trick_shot_angle = mid

%Trick shot equation function
function status = adjust(xf, zf, vf)
    status.place = 1;
    status.vx = -vf(1);
    status.vz = vf(2);
    status.x = xf;
    status.z = zf;
end

%Plotting style
function style(inp)
    axis([0, inp.dwall, 0, inp.dwall])
    line([inp.dwall inp.dwall], [0,inp.dwall], 'Color', 'k', 'LineWidth', 2)
    line([inp.dscreen inp.dscreen], [0 inp.hscreen], 'Color', 'c', "LineWidth", 2)
    hold on
    plot(inp.dbasket, 1.5, 'gp','MarkerSize', 10, 'MarkerFaceColor', 'g')
    xlabel('Distance');
    ylabel('Height');
end

%Plotting trajectory function
function [xf, zf, maxindex, zmax, vf] = draw(status, theta, inp)
    grav = 9.81;
    theta = toRadians('degrees', theta);
    tstep = inp.tolerance / 30;
    %Set initial velocity, initial position
    if status.place ==0
        v0x = inp.v0 * cos(theta);
        v0z = inp.v0 * sin(theta);
        x0 = 0; z0 = 0;
    elseif status.place ==1
        v0x = status.vx;
        v0z = status.vz;
        x0 = status.x;
        z0 = status.z;
    end  
    x(1) = x0; z(1) = z0;
    %Calculate first step
    [xt, zt] = dcalculate(v0x, v0z, tstep, x0, z0);
    [Vx, Vz] = vcalculate(v0x, v0z, inp.drag, inp.vwind, inp.mass, tstep, grav);
    x(2)= xt; z(2)= zt;
    v0x = Vx;
    v0z = Vz;
    x0 = xt;
    z0 = zt;
    i = 3;
    %Loop until ball hits ground or wall
    if status.place ==0
        while z0 - inp.tolerance/2 > 0 && x0<=25
            [xt, zt] = dcalculate(v0x, v0z, tstep, x0, z0);
            [Vx, Vz] = vcalculate(v0x, v0z, inp.drag, inp.vwind, inp.mass, tstep, grav);
            x(i) = xt;
            z(i) = zt;
            v0x = Vx;
            v0z = Vz;
            x0 = xt;
            z0 = zt;
            %Check if hit screen
            if abs(x(i)-inp.hscreen)< inp.tolerance && z(i)<=10
                break
            end
            i = i+1;
        end
    elseif status.place ==1
        while z0 - inp.tolerance/2 > 0
            [xt, zt] = dcalculate(v0x, v0z, tstep, x0, z0);
            [Vx, Vz] = vcalculate(v0x, v0z, inp.drag, inp.vwind, inp.mass, tstep, grav);
            x(i) = xt;
            z(i) = zt;
            v0x = Vx;
            v0z = Vz;
            x0 = xt;
            z0 = zt;
            %Check if hit screen
            if abs(x(i)-inp.hscreen)< inp.tolerance && z(i)<=10
                break
            end
            i = i+1;
        end
    end
    xf = x(end);
    zf = z(end);
    vf = [v0x, v0z];
    [zmax, idzmax] = max(z);
    maxindex = x(idzmax);
    plot(x, z)
end

%Calculating velocity function
function [Vx, Vz] = vcalculate(v0x, v0z, k, w, m, dt, g)
    Vx = v0x -(k/m) *dt *(v0x-w)*sqrt((v0x-w)^2 + v0z^2);
    Vz = v0z -g*dt -(k/m)*dt*(v0z*sqrt((v0x-w)^2 + v0z^2));
end

%Calculating position function
function [xt, zt] = dcalculate(v0x, v0z, dt, x0, z0)
    xt = x0 + v0x*dt;
    zt = z0 + v0z*dt;
end
