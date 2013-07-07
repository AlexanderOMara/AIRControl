AIRControl
==========

Adobe AIR Game Controller Native Extension.


Usage
-----

Everything needed to use this extension including the
ANE, SWC, AS docs, and full ActionScript source code,
can be found under `native_extension/AIRControl`.

Working example can also be found under `examples`.


Important Considerations
------------------------

Giving the user to option to configure the use of every input
element in your project is important. Every controller is different
and to avoid frustrated users, allowing them to set any axis, POV
hat switch, or button for any use is recommended. Also note that
some dual game controllers will register themselves as a single
game controller, so allowing the user complete flexibility is
necessary to maximize controller compatibility.


Minimum Requirements
--------------------

* Adobe AIR 3.1
* Flex SDK 4.6.0


Build Notes
-----------

The following binaries were built with the following compilers.

* AIRControl.framework: Xcode 4.6.2
* AIRControl.dll: Code::Blocks 10.05 with MinGW 4.4.1
* AIRControl.swc: Flash Builder 4.6 with Flex SDK 4.6.0
* AIRControl.ane: AIR SDK 3.1


Changes
-------

See CHANGELOG.md


Contributing
------------

See CONTRIBUTING.md


Licence
-------

See LICENSE.md


Donations
---------

If you find my software useful, please consider making a donation at:
[alexomara.com](http://alexomara.com)
