
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
        cover: "imgs/version/4b-xz.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i705hti"
    },
    {
        name: "究极绿宝石lV-小智版 Impossible Version【通常版修复2】",
        cover: "imgs/version/4b-xz.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i705dvg"
    }, {
        name: "究极绿宝石lV.B【Index Valution】",
        cover: "imgs/version/46-4b.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i6knpuj"
    }, {
        name: "究极绿宝石lV.A【Investment Vexes】",
        cover: "imgs/version/46-4b.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i6944ha"
    }, {
        name: "究极绿宝石lV.8【Infernal Vices 8】",
        cover: "imgs/version/46-4b.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i661aha"
    }, {
        name: "究极绿宝石lV.7【Infernal Vices 7】修复版",
        cover: "imgs/version/46-4b.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i661aad"
    }, {
        name: "究极绿宝石lV.6【Infernal Vices 6】修复版",
        cover: "imgs/version/46-4b.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i661a3g"
    }, {
        name: "究极绿宝石lV.5【Infinite Verges 5】",
        cover: "imgs/version/2-45.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i661a0d"
    }, {
        name: "究极绿宝石lV.4【Ingressive Volition】",
        cover: "imgs/version/2-45.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i6619xa"
    }, {
        name: "究极绿宝石lV.2【Inevitable Version】",
        cover: "imgs/version/2-45.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i6619qd"
    }, {
        name: "究极绿宝石lV.0【Inevitable Version】",
        cover: "imgs/version/2-45.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i6619na"
    }, {
        name: "究极绿宝石lll小修复",
        cover: "imgs/version/2-45.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i6619kh"
    }, {
        name: "究极绿宝石lll",
        cover: "imgs/version/2-45.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i6619he"
    }, {
        name: "究极绿宝石ll【Destroy Former】",
        cover: "imgs/version/2-45.png",
        downloadUrl: "https://tanyiqu.lanzous.com/i6619gd"
    }, {
        name: "更历史的版本暂时没有搜到 有请在右下角那里联系我",
        cover: "imgs/version/more.png",
        downloadUrl: "https://tieba.baidu.com/f?kw=%E7%A9%B6%E6%9E%81%E7%BB%BF%E5%AE%9D%E7%9F%B3&ie=utf-8"
    }];


/**
 * 加载版本信息
 */
function loadVersions() {

    let versionList = $('#versionList');
    let html = "";
    html += `
    <li>
        <img class="ver-img"  src="imgs/version/more.png" alt="">
        <div class="right">
            <h3 class="version-name">GBA模拟器</h3>
            <span class="version-dl" durl="https://www.bilibili.com/video/BV1Sh411o7UK">Windows模拟器</span>
            <span class="version-dl" style="margin-left:1.875rem" durl="https://www.bilibili.com/video/BV1Bp4y1i794">Android模拟器</span>
            <span class="version-dl" style="margin-left:3.625rem" durl="https://www.bilibili.com/video/BV1Ba4y1E7y2">IOS模拟器</span>
            <span class="version-dl" style="margin-left:4.875rem" durl="https://tanyiqu.lanzoui.com/b0cqgy6sd">下载所有模拟器</span>
        </div>
    </li>
    `;
    let length = VERSIONS.length;
    for (let i = 0; i < length; i++) {
        html += `
        <li>
            <img class="ver-img"  src="{1}" alt="">
            <div class="right">
                <h3 class="version-name">{2}</h3>
                <p class="version-dl" durl="{3}">下载该版本</p>
            </div>
        </li>
        `.format(i, VERSIONS[i].cover, VERSIONS[i].name, VERSIONS[i].downloadUrl);
    }
    versionList.html(html);
}


// 事件
function loadEvents() {
    // 图片点击 => 新页面显示此图片
    $('.ver-img').click((e) => {
        window.open(e.target.src);
    });


    // 点击下载
    $('.version-dl').click((e) => {
        window.open($(e.target).attr('durl'));
    });


    // 捐助图标移入移出
    $('.donate').mouseover(function () {
        $('.donate-qr').stop();
        $('.donate-qr').show(500);
    });
    $('.donate-qr').mouseout(function () {
        $('.donate-qr').stop();
        $('.donate-qr').hide(500);
    });


    // 联系我
    $('.contact').click(() => {
        alert('GitHub：Tanyiqu\n邮箱：1953649096@qq.com（加好友也行）');
    });
}