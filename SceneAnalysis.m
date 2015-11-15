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

% CCD sensor
imageSectionHeight = 12.288; 
CCDqe=[0.09 0.28 0.22 0.135]; % wavelengths = [400 500 600 700] nm

% Lens system
Dsr=0.005; %Effective lens entrance aperture
EFL=0; %Effective focal length
alphaLens=0.98; %Pass ban efficiency of lens systm
r=3; %[2,4]
DoF = 4-2;
FOV = 30; %30?x30?
f = imageSectionHeight/2/tan(FOV*d2r);

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
lambda=550 %[400,700]


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
noiseMin=LuminousPowerTowardCamMin*shutterTime/(h*c/lambda); %[]
noiseMax=LuminousPowerTowardCamMax*shutterTime/(h*c/lambda); %[]

%number of noise photons to Lens
nphotnoiseCCDMin=(pi*(Dsr/2)^2)/(2*pi*r^2)*noiseMin;
nphotnoiseCCDMax=(pi*(Dsr/2)^2)/(2*pi*r^2)*noiseMax;

%Number of noise electrons registered by CCD
nenCCDMin=nphotnoiseCCDMin*CCDqe*alphaLens;
nenCCDMax=nphotnoiseCCDMax*CCDqe*alphaLens;