(()=>{"use strict";var e,a,f,d,c,t,b,r={},o={};function l(e){var a=o[e];if(void 0!==a)return a.exports;var f=o[e]={id:e,loaded:!1,exports:{}};return r[e].call(f.exports,f,f.exports,l),f.loaded=!0,f.exports}l.m=r,l.amdO={},e=[],l.O=(a,f,d,c)=>{if(!f){var t=1/0;for(n=0;n<e.length;n++){for(var[f,d,c]=e[n],b=!0,r=0;r<f.length;r++)(!1&c||t>=c)&&Object.keys(l.O).every((e=>l.O[e](f[r])))?f.splice(r--,1):(b=!1,c<t&&(t=c));if(b){e.splice(n--,1);var o=d();void 0!==o&&(a=o)}}return a}c=c||0;for(var n=e.length;n>0&&e[n-1][2]>c;n--)e[n]=e[n-1];e[n]=[f,d,c]},l.n=e=>{var a=e&&e.__esModule?()=>e.default:()=>e;return l.d(a,{a}),a},f=Object.getPrototypeOf?e=>Object.getPrototypeOf(e):e=>e.__proto__,l.t=function(e,d){if(1&d&&(e=this(e)),8&d)return e;if("object"==typeof e&&e){if(4&d&&e.__esModule)return e;if(16&d&&"function"==typeof e.then)return e}var c=Object.create(null);l.r(c);var t={};a=a||[null,f({}),f([]),f(f)];for(var b=2&d&&e;"object"==typeof b&&!~a.indexOf(b);b=f(b))Object.getOwnPropertyNames(b).forEach((a=>t[a]=()=>e[a]));return t.default=()=>e,l.d(c,t),c},l.d=(e,a)=>{for(var f in a)l.o(a,f)&&!l.o(e,f)&&Object.defineProperty(e,f,{enumerable:!0,get:a[f]})},l.f={},l.e=e=>Promise.all(Object.keys(l.f).reduce(((a,f)=>(l.f[f](e,a),a)),[])),l.u=e=>(({938:"locale-HR",2069:"locale-PL",2240:"locale-IT",2448:"locale-SE",3442:"locale-SR",3655:"locale-SL",4340:"focus-visible-polyfill",4558:"fetch-polyfill",4877:"locale-TR",5279:"cefController",5520:"playersTidal",5652:"locale-FR",5664:"locale-ES",6346:"locale-DK",6455:"playersDesktop",7069:"locale-PT",7294:"intersection-observer-polyfill",7310:"locale-BG",7409:"cloudQueueSagas",7626:"locale-NO",7737:"resize-observer-polyfill",7858:"locale-DE",7901:"remotePlaybackSagas",9962:"locale-PT-BR"}[e]||e)+"."+{147:"c1a1183ca408ac4d7c61",601:"229803f93d5b1ff817c0",737:"0453b38ca36f1a521274",938:"32f231524a515213ade6",1131:"ad4dbf4809c79547f183",1240:"4d4abaa7d6202ea18402",1375:"21d60a73fd02640633e3",1438:"6e152bb092e340e0e64a",1502:"470ef13aff132f4f2be4",1572:"6cd0c9a1ddf091252689",1766:"fff987c1868d25148a56",1921:"68b2780b488d15a3a00f",1996:"172f55ccae8192dac8d6",2032:"8881eaba3eeb359aab67",2069:"7c27f172bf7943bba1d1",2216:"ca17de3f5f0d873f6d2b",2240:"8bf9981adcbf7a05217e",2346:"e3251f20f77eb445c3ad",2448:"fb0ec62d1cedec5c6559",2477:"abc07dd8fd0f49b2505d",2512:"ea35606595bc1be83c73",2578:"0b664562c83f9acfae79",2607:"f7bfbdaa784754ab8df3",2701:"65dfb8358cd4766ff19f",2794:"c9f4ba2eb9605f7c1a8d",2907:"969a0d0ca4b783f847a1",3418:"7e51c0d21007d7329e41",3442:"bc56dce9b1efb3e14d06",3487:"5043d9c8667725701361",3530:"4d7f671f36446c24c92c",3602:"d9efc12772c4795985e3",3655:"f153fdf6f3248d06897e",3771:"b520e7d95f3bcdac1afd",4070:"3c05a4e3b8da93c7b57f",4340:"bee45012b681c536c45e",4558:"0c6098c23a533296397c",4576:"16f162ee683f9fdf0905",4648:"7a6ca0b7b94b3c0d81d0",4844:"fde138d74ee2457c447b",4877:"736dde568645b2e49955",5279:"32f30bbe35f8cc9241cd",5284:"4f0f72b72a505e75ccb9",5370:"c28b2a934cf69e8bdd44",5506:"46fe60c926405b885a06",5520:"ee3d9651f9bd96464ec1",5624:"824d2ea07c4e15b26c81",5652:"f57e8d72a54a60761bff",5664:"ce900c307da8d835a70b",5730:"2fe8b969790b904bcff6",6032:"1763fbaea3b546053358",6150:"34176239bfe04ddf3ed8",6190:"e35cae21e09ffcbe7981",6221:"6226ff63c0f85507c49b",6346:"aefbe5cba44ed6b8fe7d",6380:"a3a68372d4cb64426a10",6455:"f51c64cd6f51cdc07669",6729:"bec08b747ef8fe46ff9b",6759:"2e391efc817a8e16839f",6944:"13672ac0ddabe93f321e",6967:"87a5148855db6ac8d574",7053:"2d940e49c331ac03d03c",7069:"43f2e6670ddb33c6e97e",7168:"595d13c97e57ed00aff4",7180:"d397b762287e4b6c472d",7294:"d8703a1d9a4e7676e6d8",7306:"a720d71c4ee731be7af9",7310:"f9223a9a4cb4b84e165c",7341:"1f44ba4427b686dfdc5d",7371:"322a78b15d8574cee6dd",7409:"ab045a3440d20a885d46",7626:"3f2950454027cac78a60",7669:"fceabb5bffe8032b6b25",7737:"77fd286e935ecf31e25e",7858:"38312828d7717dbdccd3",7901:"51c0e79fc2e3bbf1888e",7967:"c445116e584cf8136704",8132:"9b587f7fff4f28fb31df",8343:"59b8e4ef48998d00a152",8661:"fff6ea15937bd38dece5",8711:"524093c868ce8f1e00c2",8825:"37499186e009924df7a5",9644:"e9f753909bfcf32e8980",9804:"366113a48b29fc9f6a45",9840:"861813df2a1563753006",9908:"f8d9627a78d2c25fa409",9947:"f3b8ee5ec734ab5cb940",9952:"6d223181013fd6df9925",9962:"9b803d81f4773c54afea"}[e]+".chunk.js"),l.miniCssF=e=>(({1792:"standalonePage",2143:"app",3974:"head"}[e]||e)+"."+{601:"e6f1c71ac1f8f883e2f4",1375:"10c8b9f37c744a633ff8",1438:"407eeca206bb0b0fff24",1792:"4d3f79cc268b0fd72b03",1921:"15bdbdf1ca5c360050b5",1996:"6c280adbc819e394bb6d",2143:"0c7503ee454605deb24c",2216:"e6f1c71ac1f8f883e2f4",2346:"f4c17a8aaddd437a441c",2607:"f456fd4870d0044f731a",2794:"af7316be96cd00bb66c0",2907:"601fedb52ba771be0a1e",3487:"16930392c36cd924fe22",3771:"6f58add45ad889799c4a",3974:"caac450c9daeba1fe7d3",5284:"5dd7e333c5c009cac296",5506:"309c429db4a636a37829",6032:"10c8b9f37c744a633ff8",6380:"24fba46e5f2a6f3fadff",6759:"46ba686df82de7577d21",6967:"80f338c0fdc92a87a9a9",7168:"10c8b9f37c744a633ff8",7180:"105005d04cc9ee3dcf0a",7306:"6456fef428568666b6ee",7669:"6b43953f4be645240f3e",8343:"7935536d8910d79e093d",8825:"e0edd47c5c4506f19f42",9644:"99329edb445dc4fa7802",9804:"50cc40f11d5648012ad1",9840:"b5714f482c5f6f8f3dea",9952:"c275d58baf43a5994ddd"}[e]+".css"),l.g=function(){if("object"==typeof globalThis)return globalThis;try{return this||new Function("return this")()}catch(e){if("object"==typeof window)return window}}(),l.o=(e,a)=>Object.prototype.hasOwnProperty.call(e,a),d={},c="@tidal/web:",l.l=(e,a,f,t)=>{if(d[e])d[e].push(a);else{var b,r;if(void 0!==f)for(var o=document.getElementsByTagName("script"),n=0;n<o.length;n++){var i=o[n];if(i.getAttribute("src")==e||i.getAttribute("data-webpack")==c+f){b=i;break}}b||(r=!0,(b=document.createElement("script")).charset="utf-8",b.timeout=120,l.nc&&b.setAttribute("nonce",l.nc),b.setAttribute("data-webpack",c+f),b.src=e),d[e]=[a];var s=(a,f)=>{b.onerror=b.onload=null,clearTimeout(u);var c=d[e];if(delete d[e],b.parentNode&&b.parentNode.removeChild(b),c&&c.forEach((e=>e(f))),a)return a(f)},u=setTimeout(s.bind(null,void 0,{type:"timeout",target:b}),12e4);b.onerror=s.bind(null,b.onerror),b.onload=s.bind(null,b.onload),r&&document.head.appendChild(b)}},l.r=e=>{"undefined"!=typeof Symbol&&Symbol.toStringTag&&Object.defineProperty(e,Symbol.toStringTag,{value:"Module"}),Object.defineProperty(e,"__esModule",{value:!0})},l.nmd=e=>(e.paths=[],e.children||(e.children=[]),e),l.p="/",t=e=>new Promise(((a,f)=>{var d=l.miniCssF(e),c=l.p+d;if(((e,a)=>{for(var f=document.getElementsByTagName("link"),d=0;d<f.length;d++){var c=(b=f[d]).getAttribute("data-href")||b.getAttribute("href");if("stylesheet"===b.rel&&(c===e||c===a))return b}var t=document.getElementsByTagName("style");for(d=0;d<t.length;d++){var b;if((c=(b=t[d]).getAttribute("data-href"))===e||c===a)return b}})(d,c))return a();((e,a,f,d)=>{var c=document.createElement("link");c.rel="stylesheet",c.type="text/css",c.onerror=c.onload=t=>{if(c.onerror=c.onload=null,"load"===t.type)f();else{var b=t&&("load"===t.type?"missing":t.type),r=t&&t.target&&t.target.href||a,o=new Error("Loading CSS chunk "+e+" failed.\n("+r+")");o.code="CSS_CHUNK_LOAD_FAILED",o.type=b,o.request=r,c.parentNode.removeChild(c),d(o)}},c.href=a,document.head.appendChild(c)})(e,c,a,f)})),b={3666:0},l.f.miniCss=(e,a)=>{b[e]?a.push(b[e]):0!==b[e]&&{601:1,1375:1,1438:1,1921:1,1996:1,2216:1,2346:1,2607:1,2794:1,2907:1,3487:1,3771:1,5284:1,5506:1,6032:1,6380:1,6759:1,6967:1,7168:1,7180:1,7306:1,7669:1,8343:1,8825:1,9644:1,9804:1,9840:1,9952:1}[e]&&a.push(b[e]=t(e).then((()=>{b[e]=0}),(a=>{throw delete b[e],a})))},(()=>{var e={3666:0};l.f.j=(a,f)=>{var d=l.o(e,a)?e[a]:void 0;if(0!==d)if(d)f.push(d[2]);else if(/^(221|366)6$/.test(a))e[a]=0;else{var c=new Promise(((f,c)=>d=e[a]=[f,c]));f.push(d[2]=c);var t=l.p+l.u(a),b=new Error;l.l(t,(f=>{if(l.o(e,a)&&(0!==(d=e[a])&&(e[a]=void 0),d)){var c=f&&("load"===f.type?"missing":f.type),t=f&&f.target&&f.target.src;b.message="Loading chunk "+a+" failed.\n("+c+": "+t+")",b.name="ChunkLoadError",b.type=c,b.request=t,d[1](b)}}),"chunk-"+a,a)}},l.O.j=a=>0===e[a];var a=(a,f)=>{var d,c,[t,b,r]=f,o=0;if(t.some((a=>0!==e[a]))){for(d in b)l.o(b,d)&&(l.m[d]=b[d]);if(r)var n=r(l)}for(a&&a(f);o<t.length;o++)c=t[o],l.o(e,c)&&e[c]&&e[c][0](),e[t[o]]=0;return l.O(n)},f=self.webpackChunk_tidal_web=self.webpackChunk_tidal_web||[];f.forEach(a.bind(null,0)),f.push=a.bind(null,f.push.bind(f))})()})();
//# sourceMappingURL=runtime.27efc06c70aaea141c56.js.map