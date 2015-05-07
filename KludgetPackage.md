

# Kludget Package #

The most basic Kludget package is a ZIP archive that contains just two files. An HTML file and a configuration file (`kludget.xml`).

A widget package may also contain a `menu.xml` file that defines its context menu. And a `preference.xml` file that defines options for the preference window.

That package may also of course, contain widget resources such as images, css, javascript, xml.

## The HTML file ##

It is simply just an HTML file. How it is rendered on the web browser is display the same way be the Engine.

Read about `<content/>` below.

## The Configuration File ##

`kludget.xml`

This is the very first file the Engine will look for. It is an XML file containing all the configurations of the widget.

A minimum configuration file would look like:

```
<widget id="com.kludgets.minimum">
   <content src="index.html"/>
</widget>
```

### widget ###

```
<widget/>
```

The `widget` is the root element that must be present. Its attributes are:

| **Attribute** | **Required** | **Description** |
|:--------------|:-------------|:----------------|
|id|yes|This uniquely identifies the widget. This comes in the form of a reverse domain identifier. Example. `com.kludgets.clock`. This attribute is required because without a unique identifier widgets may clash and overwrite each other.|
|name|no|This is a human readable name or title for the widget|
|version|no|The version of widget. Optional, but recommended as it will facilitate automatic updates|
|width|no|The width in pixels which the widget would be displayed.|
|height|no|The heightin int pixels which the widget would be displayed.|
|debug|no|Value is 1 or 0. 1 means the widget will run in debug mode.|

### content ###

```
<widget>
   <content/>
</widget>
```

The `content` element will tell the engine the location of the start-up page of the widget. This element is also required. Its attributes:

| **Attribute** | **Required** | **Description** |
|:--------------|:-------------|:----------------|
|src|yes|The file path of the start-up page.|

Although it is recommended that the startup file is an HTML file. It is possible to use an image, a Flash document, SVG document, or any type of document that a webkit-based browser supports.

### access ###

```
<widget>
   <access/>
</widget>
```

This element specifies the security settings of the widget. This is optional but leaving this out will disable access to all resources other than that contained in the package.

Attribute values may be "true" or "false".

| **Attribute** | **Description** |
|:--------------|:----------------|
| local | Access local files outside of the package. |
| network | Access the internet. |
| plugin | Enable internet plugins such as Flash. |
| system | Enable system commands or the `widget.system()` method. |

### description ###

```
<widget>
   <description/>
</widget>
```

Describe the function of the widget here.

### author ###

```
<widget>
   <author/>
</widget>
```

This contains information about the widget creator. The value of the `author` element is the name of the author. Email, website information may be added through specifying author attributes.

| **Attribute** | **Description** |
|:--------------|:----------------|
| email | Email address of the author |
| href | Url of the author's website |

### menu ###

```
<widget>
   <menu/>
</widget>
```

This declares custom context menu items. See information about `menu.xml` below for details.

### preferences ###

```
<widget>
   <preferences/>
</widget>
```

This declares preference settings for the widget. See information about `preferences.xml` below for details.

## Menu File ##

`menu.xml`

_Todo_

## Preferences File ##

`preferences.xml`

_Todo_