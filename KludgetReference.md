

# Widget Object #

Every instance of a widget is provided a javascript object for which to control it. It is called "widget".

## Properties ##

### identifier ###

` widget.identifier `

This is the identifier of the widget. This identifier is the same as the CFBundleIdentifier in the Info.plist file. Or the `<widget id="identifier">` in the Kludget.xml file.

### instance ###

` widget.instance `

This instance identifier uniquely identifies each widget instance.

### ondragstart ###

` widget.ondragstart `

This contains the event handler to be called when a widget begins to be dragged.

```

widget.ondragstart = function()
{
}

```

### ondragend ###

` widget.ondragend `

This contains the event handler to be called when a widget is dropped or stops being dragged.

### onhide ###

` widget.onhide `

This contains the event handler to be called when a widget is hidden from the desktop or no longer visible. Put your widget in sleep mode to conserve resources.

### onremove ###

` widget.onremove `

This contains the event handler to be called when a widget is closed or removed.

The Kludget engine will remove all preference settings for the removed widget instance. You may still save some persistent information in the [database storage](http://dev.w3.org/html5/webstorage/#databases) which is shared by all widget instances and won't be deleted until the package is uninstalled.

### onsettingschanged ###

` widget.onsettingschanged `

This contains the event handler to be called when a user changes the widget instance settings or the kludget engine preferences.

### onshow ###

` widget.onshow `

This contains the event handler to be called when a widget becomes visible.

### onurlreceived ###

` widget.onurlreceived `

This contains the event handler to be called when a widget receives a url through a drag-and-drop action.

```

widget.onurlreceived = function(url)
{
}

```

## Methods ##

### about ###

` widget.about() `

Displays the widget information window. The "about" window also contains the security settings of the widget.

### close ###

` widget.close() `

Removes the widget instance. This also clears all preference settings.

` widget.configure([category]) `

Shows the widget preference window. You may optionally pass a category name to set the initial category page to be displayed.

### createInstance ###

` widget.createInstance([id]) `

Creates a new instance of the widget. You may optionally pass a widget instance ID to be used by the widget. If the ID is already used, no new instance is created.

### hide ###

` widget.hide() `

Hides the widget from the desktop view.

### inspect ###

` wigdget.inspect() `

Displays the WebKit Inspector window. This is useful for debugging errors, tracing all "alerts", and viewing the actual DOM hierarchy.

### move ###

` widget.move(x, y) `

Move the widget window. Move can also be done by manipulating the javascript window object.

### openURL ###

` widget.openURL(url) `

Opens the url with the system's default url handler.

### prepareForTransition ###

` widget.prepareForTransition(transition) `

Informs the Kludget engine that transition animation is about to be performed.

This freezes all rendering and interaction of the widget. And a snapshot of the widget is taken. This snapshot image will be used when `performTransition` is called.

### performTransition ###

` widget.performTransition() `

Performs transition animation.

### renderLayer ###

` widget.renderLayer(layer) `

This takes a snapshot of the widget and saves it to the passed layer. Available layers are `background` and `foreground`. Once rendered, the elements may be hidden or unloaded to reduce resource usage.

The `clock` widget makes use of the feature. It pre-renders the clock face, minute and hour hands to the background whenever they change. It then renders the second hand separately as it moves. This reduces SVG rendering load.

### resize ###

` widget.resize(w, h) `

Resize the widget window. Resizing can also be done by manipulating the javascript window object.

### show ###

` widget.show() `

Put back to view a hidden widget. This also raises the widget's window level to the top.

### system ###

` widget.system(command, handler) `

Executes a system command-line utility or an application. The commands must contain absolute paths.

If no `handler` is provided. The command is executed synchronously. Execution will only be limited to 60 seconds, otherwise the engine will forcibly terminated the execution.

The function returns a command object with the following properties.

| **Property** | **Description** |
|:-------------|:----------------|
| outputString | The output of the command. (`stdout`) |
| errorString | The error of the command, if any. (`stderr`) |
| status | The exit status of the command. |

If a `handler` is provided. The command is executed asynchronously. And `handler` function will be called when execution is done. The command object will be passed as a paramater.

```

function onSystemExecDone(obj)
{
}

```

The for asynchronous calls, the system object will have the following properties and methods available.

| **Property** | **Description** |
|:-------------|:----------------|
| outputString | The output of the command. (`stdout`) |
| errorString | The error of the command, if any. (`stderr`) |
| status | The exit status of the command. |
| onreadoutput | Contains an event handler to be called whenever an `outputString` is available. The output string will be passed to the handler. |
| onreaderror| Contains an event handler to be called whenever an `errorString` is available. The error string will be passed to the handler. |

| **Method** | **Description** |
|:-----------|:----------------|
| cancel() | Cancels the execution of the command. |
| write(string) | Write to `stdin`. |
| close() | Closes `stdin` and `stdout`. |

In Windows, the system command may be run inside a shell such as `bash` or `sh`. Provided Cygwin or a similar environment is available, most system commands may now be run in Windows.