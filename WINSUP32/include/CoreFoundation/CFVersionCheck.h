/*	CFVersionCheck.h
	Copyright (c) 2003-2012, Apple Inc. All rights reserved.
*/

#include <CoreFoundation/CFBase.h>

/* These functions are for checking version of the current executable for workaround purposes. Examples of usage:

Check for any version of ExampleApp:
  if (_CFAppVersionCheck(CFSTR("com.apple.ExampleApp"), NULL)) { ... workaround ... }

Check for version 7b2 of ExampleApp:
  if (_CFAppVersionCheck(CFSTR("com.apple.ExampleApp"), CFSTR("7b2"))) { ... workaround ... }

Check for version of ExampleApp linked on a pre-Tiger system:
  if (_CFAppVersionCheckLessThan(CFSTR("com.apple.ExampleApp"), CFSystemVersionTiger, -1)) { ... workaround ... }

Check for version of ExampleApp with version number less than 3.5
  if (_CFAppVersionCheckLessThan(CFSTR("com.apple.ExampleApp"), -1, 3.5)) { ... workaround ... }

Check for version of ExampleApp with version number less than than 3.5 AND linked on a pre-Tiger system (checking both is uncommon):
  if (_CFAppVersionCheckLessThan(CFSTR("com.apple.ExampleApp"), CFSystemVersionTiger, 3.5)) { ... workaround ... }
*/


/* This function returns true if the current application has the specified bundleID and has the exact same version string specified in versString.  This function caches the bundle info and is fast after being used once in an app, so no need to cache results at call sites.

bundleID       Must be non-NULL; application bundle id
versionString  If not NULL, version (CFBundleShortVersionString treated as a string) must match this. 
                 Note that this is the marketing version, CFBundleShortVersionString, and 
                 not the build version, CFBundleVersion. Pass NULL here to just check bundleID.
*/
CF_EXPORT Boolean _CFAppVersionCheck(CFStringRef bundleID, CFStringRef versionString);


/* This function returns true if the current application has the specified bundleID, is linked before linkedOnAnOlderSystemThan, and has a version number lower than versionNumberLessThan. 

Basically the arguments are those which indicate when we expect the app to be fixed (specified as either a release, or a version number) so this workaround is not needed anymore.

This function caches the bundle info and is fast after being used once in an app, so no need to cache results at call sites.

bundleID                    Must be non-NULL; application bundle id
linkedOnAnOlderSystemThan   A valid CFSystemVersion, or if no linkage check needed, -1
versionNumberLessThan       Version (CFBundleShortVersionString treated as a floating point number) must be 
			      less than this. Note that this is the marketing version, CFBundleShortVersionString, 
                              and not the build version, CFBundleVersion. Pass -1 if version check not needed.
*/
CF_EXPORT Boolean _CFAppVersionCheckLessThan(CFStringRef bundleID, int /* CFSystemVersion */ linkedOnAnOlderSystemThan, double versionNumberLessThan);

#if TARGET_OS_WIN32
/* On Windows, some applications which link against CF do not have an Info.plist. These versions of the app check functions compare against the "file version" number stored in the Windows-specific resource files. The app name is the name of the executable, not including any path, and including the .exe extension. */

/* This function returns true if the current application has the specified appName and has the exact same version number specified in 'majorVersion', 'minorVersion', 'patchVersion', and 'buildVersion'.
 */
CF_EXPORT Boolean _CFAppWindowsVersionCheck(CFStringRef appName, uint16_t majorVersion, uint16_t minorVersion, uint16_t patchVersion, uint16_t buildVersion);

/* This function returns true if the current application has the specified appName and has a version number lower than the version number specified in 'majorVersion', 'minorVersion', 'patchVersion', and 'buildVersion'. The numbers are checked from major to build. For example:
 1.999.999.999 < 2.0.0.0 (result is true)
 210.555.867.5309 < 210.556.867.5309 (result is true)
 3.14.15.9 == 3.14.15.9 (result is false)
 */
CF_EXPORT Boolean _CFAppWindowsVersionCheckLessThan(CFStringRef appName, uint16_t majorVersion, uint16_t minorVersion, uint16_t patchVersion, uint16_t buildVersion);

#endif
