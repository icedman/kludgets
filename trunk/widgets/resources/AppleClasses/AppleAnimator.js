/*

File: AppleAnimator.js

Abstract: Standard Apple class for a timed animation task; included in this
	widget under an "AppleClasses" directory for compatibility with Mac OS X
	10.4.0-10.4.2.  On Mac OS X 10.4.3 and later, Dashboard will first check
	in /System/Library/WidgetResources when loading script references to the
	AppleClasses directory.

Version: 2.0

© Copyright 2005 Apple Computer, Inc. All rights reserved.

IMPORTANT:  This Apple software is supplied to 
you by Apple Computer, Inc. ("Apple") in 
consideration of your agreement to the following 
terms, and your use, installation, modification 
or redistribution of this Apple software 
constitutes acceptance of these terms.  If you do 
not agree with these terms, please do not use, 
install, modify or redistribute this Apple 
software.

In consideration of your agreement to abide by 
the following terms, and subject to these terms, 
Apple grants you a personal, non-exclusive 
license, under Apple's copyrights in this 
original Apple software (the "Apple Software"), 
to use, reproduce, modify and redistribute the 
Apple Software, with or without modifications, in 
source and/or binary forms; provided that if you 
redistribute the Apple Software in its entirety 
and without modifications, you must retain this 
notice and the following text and disclaimers in 
all such redistributions of the Apple Software. 
Neither the name, trademarks, service marks or 
logos of Apple Computer, Inc. may be used to 
endorse or promote products derived from the 
Apple Software without specific prior written 
permission from Apple.  Except as expressly 
stated in this notice, no other rights or 
licenses, express or implied, are granted by 
Apple herein, including but not limited to any 
patent rights that may be infringed by your 
derivative works or by other works in which the 
Apple Software may be incorporated.

The Apple Software is provided by Apple on an "AS 
IS" basis.  APPLE MAKES NO WARRANTIES, EXPRESS OR 
IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY 
AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING 
THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE 
OR IN COMBINATION WITH YOUR PRODUCTS.

IN NO EVENT SHALL APPLE BE LIABLE FOR ANY 
SPECIAL, INDIRECT, INCIDENTAL OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, 
REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION OF 
THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER 
UNDER THEORY OF CONTRACT, TORT (INCLUDING 
NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN 
IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

*/ 

/*
 Copyright © 2005, Apple Computer, Inc.  All rights reserved.
 NOTE:  Use of this source code is subject to the terms of the Software
 License Agreement for Mac OS X, which accompanies the code.  Your use
 of this source code signifies your agreement to such license terms and
 conditions.  Except as expressly granted in the Software License Agreement
 for Mac OS X, no other copyright, patent, or other intellectual property
 license or right is granted, either expressly or by implication, by Apple.
 */

function AppleAnimator (duration, interval, optionalFrom, optionalTo, optionalCallback)
{	
	this.startTime = 0;
	this.duration = duration;
	this.interval = interval;
	this.animations = new Array;
	this.timer = null;
	this.oncomplete = null;
	this._firstTime = true;
	
	var self = this;
	
	this.animate = function (self) {
		
		function limit_3 (a, b, c) {
    		return a < b ? b : (a > c ? c : a);
		}
		
		var T, time;
		var ease;
		var time  = (new Date).getTime();
		var dur = self.duration;
		var done;
				
		T = limit_3(time - self.startTime, 0, dur);
		time = T / dur;
		ease = 0.5 - (0.5 * Math.cos(Math.PI * time));
		
		done = T >= dur;
		
		var array = self.animations;
		var c = array.length;
		var first = self._firstTime;
		
		for (var i = 0; i < c; ++i)
		{
			array[i].doFrame (self, ease, first, done, time);
		}
		
		if (done)
		{
			self.stop();
			DEBUG("done: oncomplete: " + self.oncomplete);
			if  (self.oncomplete != null)
			{
				self.oncomplete();
			}
		}
		
		self._firstTime = false;
	}

	if (optionalFrom !== undefined && optionalTo !== undefined && optionalCallback !== undefined)
	{
		DEBUG("creating default animation");
		this.addAnimation(new AppleAnimation (optionalFrom, optionalTo, optionalCallback));
	}
}

AppleAnimator.prototype.start = function () {
	if (this.timer == null)
	{
		var timeNow = (new Date).getTime();
		var interval = this.interval;
		
		this.startTime = timeNow - interval; // see it back one frame
		
		this.timer = setInterval (this.animate, interval, this);
	}
}

AppleAnimator.prototype.stop = function () {
	if (this.timer != null)
	{
		clearInterval(this.timer);
		this.timer = null;
	}
}

AppleAnimator.prototype.addAnimation = function (animation) {

	this.animations[this.animations.length] = animation;
}

//
// Animation class
//

function AppleAnimation (from, to, callback)
{
	this.from = from;
	this.to = to;
	this.callback = callback;
	DEBUG("from: " + from + " to: " +  to + " callback: " + callback);
	this.now = from;
	this.ease = 0;
	this.time = 0;
}

AppleAnimation.prototype.doFrame = function (animation, ease, first, done, time) {
	
	var now;
	
	if (done)
		now = this.to;
	else
		now = this.from + (this.to - this.from) * ease;
	
	this.now = now;
	this.ease = ease;
	this.time = time;
	this.callback (animation, now, first, done);
}

//
// RectAnimation class
//

function AppleRectAnimation (from, to, callback)
{
	this.from = from;
	this.to = to;
	this.callback = callback;
	
	this.now = from;
	this.ease = 0;
	this.time = 0;
}

AppleRectAnimation.prototype = new AppleAnimation (0, 0, null);

AppleRectAnimation.prototype.doFrame = function (animation, ease, first, done, time) {
	
	var now;
	
	function computeNextRectangle (from, to, ease)
	{
		return addRects (from, timesRect (minusRects(to, from), ease));
	}

	if (done)
		now = this.to;
	else
		now = AppleRect.add (this.from, AppleRect.multiply (AppleRect.subtract (this.to, this.from), ease));
		
		//computeNextRectangle (this.from, this.to, ease);
	this.now = now;
	this.ease = ease;
	this.time = time;
	this.callback (animation, now, first, done);
}

//
// AppleRect class
//

function AppleRect (left, top, right, bottom) {
	this.left = left;
	this.top = top;
	this.right = right;
	this.bottom = bottom;
}


AppleRect.add = function (a, b) {
	return new AppleRect (a.left + b.left,
						  a.top + b.top,
						  a.right + b.right,
						  a.bottom + b.bottom);
}

AppleRect.subtract = function (a, b) {
	return new AppleRect (a.left - b.left,
						  a.top - b.top,
						  a.right - b.right,
						  a.bottom - b.bottom);
}

AppleRect.multiply = function (a, multiplier) {
	return new AppleRect (a.left * multiplier,
						  a.top * multiplier,
						  a.right * multiplier,
						  a.bottom * multiplier);
}

// generic applier, pass methods like Math.floor/round/ceil
AppleRect.prototype.apply = function (func) {
	this.left = func(this.left);
	this.top = func(this.top);
	this.right = func(this.right);
	this.bottom = func(this.bottom);
	return this;
}

AppleRect.prototype.toString = function () {
	return "{left:" + this.left + ", top:" + this.top + ", right:" + this.right + ", bottom:" + this.bottom + "}";
}