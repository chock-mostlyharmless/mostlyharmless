function R = framedCeps(A, frameLength, frameStep)
  
  % add 0s to get whole frames. Whatever.
  if (frameLength > frameStep)
    A = [zeros(floor((frameLength-frameStep)./2), 1); A; zeros(ceil((frameLength-frameStep)./2), 1)];
  end
  
  frameEnd = length(A)-frameLength+1;
  framePoss = 1:frameStep:frameEnd;
  numFrames = length(framePoss);
  
  R = zeros(numFrames, frameLength);
  
  for fr = 1:numFrames
    frame = A(framePoss(fr):framePoss(fr)+frameLength-1);
    frame = frame - mean(frame);
    frame = frame .* hanning(frameLength);
    %frame = frame ./ sqrt(sum(frame.*frame));    
    
    %frame = log(abs(fft(frame)));
    %frame = frame(1:frameLength/2);
    %frame = dct(frame);
    %frame = real(ifft(frame));
    
    %frame = log(abs(fft(xcorr(frame, frame, 'coeff'))));
    %frame = xcorr(frame, frame, 'coeff');
    frame = xcorr(frame, [], 'coeff');
    
    %invHann = 1 ./ (hanning(length(frame)) + eps);
    %frame = frame .* invHann;
    
    R(fr, :) = frame(frameLength:end)';
  end
  
  