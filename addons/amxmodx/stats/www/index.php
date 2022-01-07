<?php
include("pre.php");
//
$ranking = (new PlayerStats)->getRank();
?>
<?php if(!empty($ranking)): ?>
<div class="row">
    <div class="col-md-12">
        <table class="table table-sm table-striped table-bordered text-center table-dark small">
            <tr>
                <th>#</th>
                <th>Player</th>
                <th><abbr title="Frags">F</abbr></th>
                <th><abbr title="Assists">A</abbr></th>
                <th><abbr title="Deaths">D</abbr></th>
                <th><abbr title="Bomb Plants">BP</abbr></th>
                <th><abbr title="Bomb Defusals">BD</abbr></th>
                <th><abbr title="2 Kill Rounds">2K</abbr></th>
                <th><abbr title="3 Kill Rounds">3K</abbr></th>
                <th><abbr title="4 Kill Rounds">4K</abbr></th>
                <th><abbr title="5 Kill Rounds">5K</abbr></th>
                <th><abbr title="Success in 1v1 Situations">1v1</abbr></th>
                <th><abbr title="Success in 1v2 Situations">v2</abbr></th>
                <th><abbr title="Success in 1v3 Situations">v3</abbr></th>
                <th><abbr title="Success in 1v4 Situations">v4</abbr></th>
                <th><abbr title="Success in 1v5 Situations">v5</abbr></th>
                <th><abbr title="Headshot Percentage">HSP</abbr></th>
                <th><abbr title="Rounds Played">RP</abbr></th>
                <th><abbr title="Average Damage Per Round">ADR</abbr></th>
                <th><abbr title="Average Frags Per Round">FPR</abbr></th>
                <th><abbr title="Average Round Win Shares">RWS</abbr></th>
                <th><abbr title="Match Wins">W</abbr></th>
                <th><abbr title="Match Losses">L</abbr></th>
                <th><abbr title="Match Win Percentage">PCT</abbr></th>
            </tr>
            <?php foreach($ranking as $row): ?>
            <tr>
                <td><?= ++$position ?></td>
                <td><a class="text-reset" href="player.php?auth=<?= $row['auth'] ?>"><?= $row['player'] ?></a></td>
                <td><?= $row['kills'] ?></td>
                <td><?= $row['assist'] ?></td>
                <td><?= $row['death'] ?></td>
                <td><?= $row['bp'] ?></td>
                <td><?= $row['bd'] ?></td>
                <td><?= $row['2k'] ?></td>
                <td><?= $row['3k'] ?></td>
                <td><?= $row['4k'] ?></td>
                <td><?= $row['5k'] ?></td>
                <td><?= $row['v1'] ?></td>
                <td><?= $row['v2'] ?></td>
                <td><?= $row['v3'] ?></td>
                <td><?= $row['v4'] ?></td>
                <td><?= $row['v5'] ?></td>
                <td><?= sprintf("%.3f",$row['hsp']) ?></td>
                <td><?= $row['rp'] ?></td>
                <td><?= sprintf("%.3f",$row['adr']) ?></td>
                <td><?= sprintf("%.3f",$row['fpr']) ?></td>
                <td><?= sprintf("%.2f",$row['rws']) ?></td>
                <td><?= $row['wins'] ?></td>
                <td><?= $row['loses'] ?></td>
                <td><?= sprintf("%.2f",$row['winPct']) ?></td>
            </tr>
            <?php endforeach; ?>
        </table>
    </div>
</div>
<?php endif; ?>
<?php
include("pro.php");