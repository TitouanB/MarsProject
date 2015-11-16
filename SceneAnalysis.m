clear all; close all; clc;

% Natural constants:
r2d = 180/pi;
d2r=pi/180;
h=6.6263*10^-34; %Planck's constant
c=3*10^8; %Velocity if light
k=1.38062*10^-23; %Boltzmann's constant

AtargetMin=0.01; % surface's target min [m2]
AtargetMax=0.1; % surface's target max [m2]
shutterTime=100*10^-3; % shutter time [s]

lambda=[400 500 600 700]*10^(-9); % [400,700] nm

% CCD sensor
imageSectionHeight = 12.288*10^(-3); % m
CCDqe=[0.09 0.28 0.22 0.135]; % wavelengths = [400 500 600 700] nm

% Lens system
Dsr=0.005; %Effective lens entrance aperture
EFL=0; %Effective focal length
alphaLens=0.98; %Pass ban efficiency of lens systm
r=2; %[1,2]
DoF = 2-1; % m
objectSize = 2; % m
FOVr = atan(objectSize/2/r);
FOVd = FOVr*r2d;
EFL = imageSectionHeight/2/tan(FOVr); % m

% Confusion
rf = 1;
f = 1/(1/rf+1/EFL);
Dsrtmp=[1:0.1:5]*10^(-3);
figure;
subplot(1,2,1);
DoC=Dsrtmp*abs(r-rf)/r*f/(rf-f); % Diameter of Confusion
plot(Dsrtmp, DoC);

Difspot = zeros(4,length(Dsrtmp));
hold on
for i=1:4
    Difspot(i,:) = 2*EFL*tan(1.22*lambda(i)./Dsrtmp); % Diffraction spot size on CCD
    plot(Dsrtmp, Difspot);
end
legend('DoC (m)', 'Difspot (m) lambda = 400 nm', 'Difspot (m) lambda = 500 nm', 'Difspot (m) lambda = 600 nm', 'Difspot (m) lambda = 700 nm')
xlabel('Dsr (m)')

subplot(1,2,2);
hold on
for i=1:4
    plot(Dsrtmp, Difspot(i,:)+DoC);
end
xlabel('Dsr (m)')
ylabel('DoC + Difspot (m)')
legend('lambda = 400 nm', 'lambda = 500 nm', 'lambda = 600 nm', 'lambda = 700 nm')

% Reflectance map
IrradianceSun = 530;
Latitude = 50; %latitude in degrees

obliquity = 0;%25.19; ???????????
%at solar noon
angleSunMax = 90-abs(Latitude+obliquity);
IrradianceSunMax=IrradianceSun*sin(angleSunMax*d2r);
%at min work hour
angleSunMin = 10; %arbitrary
IrradianceSunMin=IrradianceSun*sin(angleSunMin*d2r);

%Target properties
alphaMin=0.05; %albedo min
alphaMax=0.45; %albedo max

thetaMin=10; %angle between sun's beam and tagert's normal
thetaMax=50;

%BRDF : 1/10 Glossy and 9/10 Lambertian
RadianceTargetMin = IrradianceSunMin*alphaMin*(1/10+cos(thetaMax*d2r)*(9/(10*pi))); % []
RadianceTargetMax = IrradianceSunMax*alphaMax*(1/10+cos(thetaMin*d2r)*(9/(10*pi)));

LuminousPowerTowardCamMin = RadianceTargetMin*AtargetMin; %[W]
LuminousPowerTowardCamMax = RadianceTargetMax*AtargetMax; %[W]


% Noise photons per shutter time
noiseMin=LuminousPowerTowardCamMin*shutterTime./(h*c./lambda); %[]
noiseMax=LuminousPowerTowardCamMax*shutterTime./(h*c./lambda); %[]

%number of noise photons to Lens
nphotnoiseCCDMin=(pi*(Dsr/2)^2)/(2*pi*r^2).*noiseMin;
nphotnoiseCCDMax=(pi*(Dsr/2)^2)/(2*pi*r^2).*noiseMax;

%Number of noise electrons registered by CCD
nenCCDMin=nphotnoiseCCDMin.*CCDqe*alphaLens;
nenCCDMax=nphotnoiseCCDMax.*CCDqe*alphaLens;