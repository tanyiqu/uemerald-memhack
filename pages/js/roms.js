
// 入口函数
$(document).ready(() => {
    loadVersions();
});



let VERSIONS =
    [{
        name: "究极绿宝石lV.B-VGC2019神战纪念版",
        cover: "imgs/version/4b-vgc.png",
        downloadUrl: ""
    }, {
        name: "究极绿宝石lV-小智版 Impossible Version 通常版",
        cover: "imgs/version/4b-xz.png",
        downloadUrl: ""
    }];




/**
 * 加载版本信息
 */
function loadVersions() {

    let versionList = $('#versionList');
    let html = "";
    let length = VERSIONS.length;
    for (let i = 0; i < length; i++) {
        html += ' <li><img src="' + VERSIONS[i].cover + '" alt=""><div class="right"><h3 class="version-name">' + VERSIONS[i].name + '</h3></div></li>';
        console.log(VERSIONS[i].cover);
    }
    versionList.html(html);
}