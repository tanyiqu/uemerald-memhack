
// 入口函数
$(document).ready(() => {
    // 加载版本信息
    loadVersions();

    // 点击事件
    loadEvents();
});



let VERSIONS =
    [{
        name: "究极绿宝石lV.B-VGC2019神战纪念版",
        cover: "imgs/version/4b-vgc.png",
        downloadUrl: "https://tanyiqu.lanzous.com/iwwWOfttlih"
    }, {
        name: "究极绿宝石lV Ash Inverse Version【逆属性修复版】",
        cover: "imgs/version/4b-xz-nsx.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i705hti"
    },
    {
        name: "究极绿宝石lV-小智版 Impossible Version【通常版修复2】",
        cover: "imgs/version/4b-xz.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i705dvg"
    }, {
        name: "究极绿宝石lV.B【Index Valution】",
        cover: "imgs/version/4b.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i6knpuj"
    }];


/**
 * 加载版本信息
 */
function loadVersions() {

    let versionList = $('#versionList');
    let html = "";
    let length = VERSIONS.length;
    for (let i = 0; i < length; i++) {
        html += `
        <li>
            <img class="ver-img"  src="{1}" alt="">
            <div class="right">
                <h3 class="version-name">{2}</h3>
                <p class="version-dl" durl="{3}">下载该版本</p>
            </div>
        </li>`.format(i, VERSIONS[i].cover, VERSIONS[i].name, VERSIONS[i].downloadUrl);
    }
    versionList.html(html);
}


// 点击事件
function loadEvents() {
    // 图片点击 => 新页面显示此图片
    $('.ver-img').click((e) => {
        window.open(e.target.src);
    });


    // 点击下载
    $('.version-dl').click((e) => {
        window.open($(e.target).attr('durl'));
    });
}