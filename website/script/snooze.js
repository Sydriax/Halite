var snooze = {
    // SNOOZE GLOBAL FIELDS
    snoozes: [],
    pipes: {},
    guards: {},
    handlers: {},
    models: {},
    handler_map: {
        "data-click":"click",
        "data-input":"input",
        "data-change":"change",
        "data-mouseover":"mouseover",
        "data-keydown":"keydown"
    },

    // SNOOZE GLOBAL METHODS
    req: function(type, url, callback, errorCallback, data) {
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() {
            if (xhr.readyState == 4) {
                if (xhr.status == 200) callback(JSON.parse(xhr.responseText));
                else if (errorCallback !== null) errorCallback(xhr);
            }
        }; xhr.open(type, url, true);
        xhr.send(JSON.stringify(data));
    },
    minimorph: function(fromNode, toNode) {
        var savedEls = {};
        var unmatchedEls = {};

        var newDOM = document.createElement("div");
        newDOM.innerHTML = toNode;
        toNode = newDOM;

        function removeNodeHelper(node, nestedInSavedEl) {
            if (node.id) savedEls[node.id] = node;
            if (node.nodeType === 1) {
                var curChild = node.firstChild;
                while(curChild) {
                    removeNodeHelper(curChild, nestedInSavedEl || node.id);
                    curChild = curChild.nextSibling;
                }
            }
        }

        function walkDiscardedChildNodes(node) {
            if (node.nodeType === 1) {
                var curChild = node.firstChild;
                while(curChild) {
                    if (!curChild.id) walkDiscardedChildNodes(curChild);
                    curChild = curChild.nextSibling;
                }
            }
        }

        function removeNode(node, parentNode, alreadyVisited) {
            parentNode.removeChild(node);
            if (alreadyVisited) {
                if (!node.id) walkDiscardedChildNodes(node);
            } else removeNodeHelper(node);
        }

        function morphEl(fromEl, toEl, alreadyVisited) {
            if (toEl.id) delete savedEls[toEl.id];

            var foundAttrs = {};

            for (var i=toEl.attributes.length-1; i>=0; i--) {
                var attr = toEl.attributes[i];
                if (attr.specified !== false) {
                    foundAttrs[attr.name] = true;
                    if (fromEl.getAttribute(attr.name) !== attr.value) {
                        fromEl.setAttribute(attr.name, attr.value);
                    }
                }
            }

            for (var i=fromEl.attributes.length-1; i>=0; i--) {
                var attr = fromEl.attributes[i];
                if (attr.specified !== false) {
                    var attrName = attr.name;
                    if (!foundAttrs.hasOwnProperty(attrName)) {
                        fromEl.removeAttribute(attrName);
                    }
                }
            }

            var curToNodeChild = toEl.firstChild;
            var curFromNodeChild = fromEl.firstChild;
            var curToNodeId;

            var fromNextSibling;
            var toNextSibling;
            var savedEl;
            var unmatchedEl;

outer:      while(curToNodeChild) {
                toNextSibling = curToNodeChild.nextSibling;
                curToNodeId = curToNodeChild.id;

                while(curFromNodeChild) {
                    fromNextSibling = curFromNodeChild.nextSibling;

                    if (!alreadyVisited) {
                        if (curFromNodeChild.id && (unmatchedEl = unmatchedEls[curFromNodeChild.id])) {
                            unmatchedEl.parentNode.replaceChild(curFromNodeChild, unmatchedEl);
                            morphEl(curFromNodeChild, unmatchedEl, alreadyVisited);
                            curFromNodeChild = fromNextSibling;
                            continue;
                        }
                    }

                    if (curFromNodeChild.nodeType === curToNodeChild.nodeType) {
                        var isCompatible = false;

                        if (curFromNodeChild.nodeType === 1) { // Both nodes being compared are Element nodes
                            if (curFromNodeChild.tagName === curToNodeChild.tagName) {
                                if (curFromNodeChild.id || curToNodeId) {
                                    if (curToNodeId === curFromNodeChild.id) isCompatible = true;
                                } else isCompatible = true;
                            } if (isCompatible) morphEl(curFromNodeChild, curToNodeChild, alreadyVisited);
                        } else if (curFromNodeChild.nodeType === 3) { // Both nodes being compared are Text nodes
                            isCompatible = true;
                            curFromNodeChild.nodeValue = curToNodeChild.nodeValue;
                        }

                        if (isCompatible) {
                            curToNodeChild = toNextSibling;
                            curFromNodeChild = fromNextSibling;
                            continue outer;
                        }
                    }

                    // No compatible match so remove the old node from the DOM and continue trying
                    // to find a match in the original DOM
                    removeNode(curFromNodeChild, fromEl, alreadyVisited);
                    curFromNodeChild = fromNextSibling;
                }

                if (curToNodeId) {
                    if ((savedEl = savedEls[curToNodeId])) {
                        morphEl(savedEl, curToNodeChild, true);
                        curToNodeChild = savedEl; // We want to append the saved element instead
                    } else unmatchedEls[curToNodeId] = curToNodeChild;
                } fromEl.appendChild(curToNodeChild); // end of search

                curToNodeChild = toNextSibling;
                curFromNodeChild = fromNextSibling;
            }

            while(curFromNodeChild) {
                fromNextSibling = curFromNodeChild.nextSibling;
                removeNode(curFromNodeChild, fromEl, alreadyVisited);
                curFromNodeChild = fromNextSibling;
            }

            if (fromEl.tagName === "INPUT") {
                if (toEl.hasAttribute('checked') && !fromEl.hasAttribute('checked'))
                    fromEl.checked = 1;
                if (toEl.hasAttribute('value') && !fromEl.hasAttribute('value'))
                    fromEl.value = toEl.getAttribute("value");
            }
        } if (fromNode !== toNode) morphEl(fromNode, toNode, false);

        return fromNode;
    },
    setListeners: function(elements) {
        Array.prototype.forEach.call(elements,function(element) {
            for (var i=0; i<element.attributes.length; i++) {
                if (typeof(this.handler_map[element.attributes[i].nodeName]) !== "undefined") {
                    var eventName = this.handler_map[element.attributes[i].nodeName];
                    element.attributes[i].nodeValue.split(" ").forEach(function(name) {
                        if (typeof(this.handlers[name]) === "undefined") {
                            throw "snooze error: Handler \"" + name + "\" does not exist.\n  -> for event \"" + element.attributes[i].nodeName + "\"";
                        } else element.addEventListener(eventName,this.handlers[name]);
                    }.bind(this));
                }
            }
        }.bind(this));
    },
    bindToState: function(elements) {
        function addProps(obj, arr, val) {
            if (typeof arr == 'string') arr = arr.split(".");
            obj[arr[0]] = obj[arr[0]] || {};
            var tmpObj = obj[arr[0]];
            if (arr.length > 1) {
                arr.shift();
                addProps(tmpObj, arr, val);
            } else obj[arr[0]] = val;
            return obj;
        }

        Array.prototype.forEach.call(elements,function(element) {
            for (var i=0; i<element.attributes.length; i++) {
                if (element.attributes[i].nodeName === "data-model") {
                    element.attributes[i].nodeValue.split(" ").forEach(function(modelName) {
                        if (element.className.split(" ").includes("radioGroup")) {
                            element.childNodes.forEach(function(node) {
                                node.name="snooze-temp";
                            });

                            var selected = element.querySelector('input:checked');
                            var value = selected === null ? null : selected.value;
                            addProps(this.models, modelName, value);
                            element.addEventListener("change",function(e) {
                                var selected = element.querySelector('input:checked');
                                var value = selected === null ? null : selected.value;
                                addProps(this.models, modelName, value);
                            }.bind(this));
                        } else if (element.type === "checkbox" || element.type === "radio") {
                            addProps(this.models, modelName, element.checked);
                            element.addEventListener("change",function(e) {
                                addProps(this.models, modelName, element.checked);
                            }.bind(this));
                        } else {
                            addProps(this.models, modelName, element.value);
                            element.addEventListener("input",function(e) {
                                addProps(this.models, modelName, element.value);
                            }.bind(this));
                        }
                    }.bind(this));

                }
            }
        }.bind(this));
    },

    // LOCAL TEMPLATE METHODS
    withID: function(id) {
        return this.snoozes.find(function(snooze) {
            return snooze.id === id;
        });
    },
    gen: function() {
        var re = new RegExp("<~([^%>]+)?~>","g");
        var reExp = new RegExp("(^( )?(if|for|else|switch|case|break|{|}))(.*)?","g");
        var code = "var r=[];\n";
        var cursor = 0;

        function add(line, js) {
            if (js) code += line.match(reExp) ? line + '\n' : 'r.push(' + line + ');\n';
            else if (line !== '') code += 'r.push("' + line.replace(/"/g, '\\"') + '");\n';
            return add;
        }

        var match;
        while(match = re.exec(this.innerHTML)) {
            add(this.innerHTML.slice(cursor, match.index))(match[1], true);
            cursor = match.index + match[0].length;
        }

        add(this.innerHTML.substr(cursor, this.innerHTML.length - cursor));
        code += 'return r.join("");';

        try {
            var newHTML = new Function(
                code.replace(/[\r\t\n]/g, '')
            ).apply(this);
        } catch (error) {
            error.message = error.message + "\n  -> in template \"" + this.id + "\"";
            throw error;
            return;
        }

        snooze.minimorph(this.dom, newHTML);
    },
    refresh: function() {
        this.pipe.func(function(newData) {
            if (this.pipe.cache) {
                localStorage.setItem('snz_'+this.id,JSON.stringify(newData));
            }

            this.guards.forEach(function(func) {
                newData = func(newData);
            }); this.data = newData;

            this.gen();

            var elements = this.dom.getElementsByTagName("*");
            snooze.bindToState(elements);
            snooze.setListeners(elements);
        }.bind(this));
    },
    setPipe: function(pipeName, errorHandlerName, cache) {
        if (typeof(this.pipe) === "undefined") this.pipe = {};

        if (typeof(errorHandlerName) === "undefined" || errorHandlerName === null) {
            if (typeof(this.pipe.errorHandler) === "undefined")
                this.pipe.errorHandler = null;
        } else {
            var funcs = errorHandlerName.split(" ").map(function(attr) {
                if (typeof(snooze.handlers[attr]) === "undefined") {
                    throw "snooze error: Pipe error handler \"" + attr + "\" does not exist.\n  -> in template \"" + this.id + "\"";
                } else return snooze.handlers[attr];
            }.bind(this));

            this.pipe.errorHandler = function(error) {
                funcs.forEach(function(func) {
                    func(error);
                });
            }
        }

        if (pipeName.indexOf("/") != -1) {
            this.pipe.url = pipeName;
            this.pipe.func = function(callback) {
                snooze.req("GET", this.url, callback, this.errorHandler);
            }.bind(this.pipe); this.pipe.type = "url";
        } else if (typeof(snooze.pipes[pipeName]) === "object") {
            this.pipe.func = function(callback) {
                callback(snooze.pipes[pipeName]);
            }; this.pipe.type = "object";
        } else {
            if (typeof(snooze.pipes[pipeName]) === "undefined") {
                throw "snooze error: Pipe \"" + pipeName + "\" does not exist.\n  -> in template \"" + this.id + "\"";
            } else {
                this.pipe.func = snooze.pipes[pipeName];
                this.pipe.type = "custom";
            }
        };

        if (cache && typeof(cache) !== typeof(this.pipe.cache)) {
            if (typeof(localStorage.getItem('snz_'+this.id)) !== "undefined" &&
                localStorage.getItem('snz_'+this.id) !== "null") {

                this.pipe.func = function(callback) {
                    callback(JSON.parse(localStorage.getItem('snz_'+this.id)));
                }.bind(this);

                this.refresh();
                this.pipe.cache = true;
                this.setPipe(pipeName,errorHandlerName);
            }
        } this.pipe.cache = cache;

        this.refresh();
    },
    stopPoll: function() {
        clearInterval(this.intervalID);
        this.intervalID = null;
    },
    setPoll: function(period) {
        if (typeof(this.intervalID) !== "undefined" && this.intervalID !== null) this.stopPoll();

        if (period === "default") {
            if (this.pipe.type === "object") this.intervalID = setInterval(this.refresh, 300);
        } else if (!isNaN(period) && /[^\s]/.test(period))  {
            this.intervalID = setInterval(this.refresh, parseFloat(period)*1000);
        } else this.stopPoll();
    },

    // SNOOZE INITIALIZATION ROUTINE
    init: function() {
        var elements = document.body.getElementsByTagName("*");
        this.snooze.bindToState(elements);
        this.snooze.setListeners(elements);

        this.snooze.snoozes = Array.prototype.filter.call(elements, function(tag) {
            return tag.type === "text/snooze";
        });

        this.snooze.snoozes.forEach(function(snooze, i) {
            snooze.gen      = this.gen.bind(snooze);
            snooze.refresh  = this.refresh.bind(snooze);
            snooze.setPipe  = this.setPipe.bind(snooze);
            snooze.setPoll  = this.setPoll.bind(snooze);
            snooze.stopPoll = this.stopPoll.bind(snooze);

            snooze.dom = document.createElement('div');
            snooze.parentNode.insertBefore(snooze.dom, snooze.nextSibling);

            if (!snooze.hasAttribute("id")) snooze.id = "snooze_template_" + i;

            snooze.guards = [];
            if (snooze.hasAttribute("data-guard")) {
                snooze.getAttribute("data-guard").split(" ").forEach(function(guard) {
                    if (typeof(this.guards[guard]) === "undefined") {
                        throw "snooze error: Guard \"" + guard + "\" does not exist.\n  -> in template \"" + snooze.id + "\"";
                    } else snooze.guards.push(this.guards[guard]);
                }.bind(this));
            }

            if (snooze.hasAttribute("data-pipe-initial"))
                snooze.setPipe(snooze.getAttribute("data-pipe-initial"));

            snooze.setPipe(snooze.getAttribute("data-pipe"),
                           snooze.getAttribute("data-pipe-error"),
                           snooze.hasAttribute("data-pipe-cache"));

            snooze.setPoll(snooze.getAttribute("data-period") || "default");

        }.bind(this.snooze));
    }
};

window.onload = snooze.init;
